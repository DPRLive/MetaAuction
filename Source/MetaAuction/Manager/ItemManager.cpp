
#include "ItemManager.h"
#include "../Actor/ItemActor.h"

#include <Kismet/GameplayStatics.h>
#include <Interfaces/IHttpResponse.h>
#include <Serialization/JsonSerializer.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemManager)

UItemManager::UItemManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UItemManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemManager, ItemActors);
}

void UItemManager::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서 레벨에 있는 Item Actor들을 관리합니다.
	if(IsRunningDedicatedServer())
	{
		TArray<AActor*> outActors;
		UGameplayStatics::GetAllActorsOfClass(MAGetWorld(), AItemActor::StaticClass(), outActors);
		for(AActor* actor : outActors)
		{
			if(AItemActor* itemActor = Cast<AItemActor>(actor))
			{
				ItemActors.Emplace(itemActor);
			}
		}

		// 인덱스를 정렬하여 Level Position에 맞춘다.
		ItemActors.Sort([](const TWeakObjectPtr<AItemActor>& a1, const TWeakObjectPtr<AItemActor>& a2)
		{
			return a1->GetLevelPosition() < a2->GetLevelPosition();
		});
	}
}

/**
 * 상품이 삭제 되었을 때, 서버에게 ItemData 요청 후 내 월드에 있었던 상품이면 지웁니다.
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_UnregisterItem(uint32 InItemId)
{
	CHECK_DEDI_FUNC;
	
	// 선형 검색..
	for(const TWeakObjectPtr<AItemActor>& itemActor : ItemActors)
	{
		// 어 내 월드에 있던 물품이었네 -> 삭제
		if(itemActor.IsValid() && itemActor->GetItemID() == InItemId)
		{
			itemActor->Server_RemoveItem();
			break;
		}
	}
}

/**
 * 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_RegisterNewItem(uint32 InItemId)
{
	CHECK_DEDI_FUNC;
	
	TWeakObjectPtr<UItemManager> thisPtr = this;
	RequestItemDataByID([thisPtr, InItemId](const FItemData& InItemData)
	{
		// TODO: 월드 관리 어떻게 해야하지.. 일단 하드코딩
		if(thisPtr.IsValid() && InItemData.World == TEXT("1"))
		{
			thisPtr->_Server_RegisterItemByLoc(InItemId, InItemData.Location);
		}
	}, InItemId);
}


/**
 * 웹서버에 등록된 현재 월드에 배치되어 판매되고 있는 아이템들의 ID를 가져와 배치한다.
 * 데디 서버에서만 실행 가능합니다.
 */
void UItemManager::Server_RegisterAllWorldItemID()
{
	CHECK_DEDI_FUNC;
	
	// 현재 월드에서, 구매 가능하게 배치되어 있는 상품을 검색하기 위한 Body를 만든다.
	// TODO : 월드 어케 관리함 근데? 일단 1로 하드코딩
	FItemSearchOption option;
	option.World = TEXT("1");
	option.CanDeal = EItemCanDeal::Possible;

	TWeakObjectPtr<UItemManager> thisPtr = this;
	RequestItemDataByOption([thisPtr](const TArray<FItemData>& InItemData)
	{
		if(!thisPtr.IsValid())
			return;
		
		for(const FItemData& data : InItemData)
		{
			thisPtr->_Server_RegisterItemByLoc(data.ItemID, data.Location);
		}
	}, option);
}

/**
 * 클라이언트에서 ItemID로 물품 정보를 요청합니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FOnGetItemDataCallback 형태의 함수 
 * @param InItemId : 쿼리할 물품의 ID
 */
void UItemManager::RequestItemDataByID(FGetItemDataByIdCallback InFunc, uint32 InItemId)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		FString RequestUrl = DA_NETWORK(ItemInfoAddURL) + FString::Printf(TEXT("/%d"), InItemId);

		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(RequestUrl, EHttpRequestType::GET, [thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		{
			if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
			{
				// Json reader 생성
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
				TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
				FJsonSerializer::Deserialize(reader, jsonObject);

				FItemData itemData;
				thisPtr->_JsonToData(jsonObject, itemData);

				if(InFunc) // 함수 실행
				{
					InFunc(itemData);
				}
				
				LOG_N(TEXT("Get Item Data(item id : %d) Success!"), itemData.ItemID);
			}
		});
	}
}

/**
 * 옵션을 걸어 물품 정보들을 요청합니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수 
 * @param InSearchOption : 물품 검색 시 사용할 옵션, 필요한 옵션만 설정 후 넣어서 사용하면 됩니다.
 */
void UItemManager::RequestItemDataByOption(FCallbackRefArray<FItemData> InFunc, const FItemSearchOption& InSearchOption)
{
	// 옵션 설정
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	if(InSearchOption.SearchString != TEXT(""))
		requestObj->SetStringField(TEXT("searchString"), InSearchOption.SearchString.ToLower());
	
	if(InSearchOption.World != TEXT(""))
		requestObj->SetStringField(TEXT("world"), InSearchOption.World.ToLower());

	if(InSearchOption.ItemType != EItemDealType::None)
		requestObj->SetStringField(TEXT("type"), ENUM_TO_STRING(EItemDealType, InSearchOption.ItemType).ToLower());

	if(InSearchOption.CanDeal == EItemCanDeal::Possible)
		requestObj->SetBoolField(TEXT("isPossible"), true);
	else if(InSearchOption.CanDeal == EItemCanDeal::Impossible)
		requestObj->SetBoolField(TEXT("isPossible"), false);
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(DA_NETWORK(ItemSearchAddURL), EHttpRequestType::POST,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // Json reader 생성
									 TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
									 TArray<TSharedPtr<FJsonValue>> jsonValues;
									 FJsonSerializer::Deserialize(reader, jsonValues);

									 TArray<FItemData> itemDatas;
									 for (TSharedPtr<FJsonValue>& itemInfo : jsonValues)
									 {
										 TSharedPtr<FJsonObject> itemInfoObj = itemInfo->AsObject();
										 itemDatas.Emplace(FItemData());
										 thisPtr->_JsonToData(itemInfoObj, *itemDatas.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(itemDatas);
									 }
								 	
									 LOG_N(TEXT("Search Items Success!"));
								 }
							 }, httpHandler->JsonToString(requestObj));
	}
}

/**
* HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
* @param InItemId : 입찰할 item의 id
* @param InPrice : 입찰할 가격
*/
void UItemManager::Client_RequestBid(uint32 InItemId, uint64 InPrice)
{
	if (IsRunningDedicatedServer()) return; // 데디면 하지 마라.

	// Body를 만든다.
	FString requestBody = FString::Printf(TEXT("%llu"), InPrice);

	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		httpHandler->Request(DA_NETWORK(BidAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::POST,
		                     [](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		                     {
			                     if (InbWasSuccessful && InResponse.IsValid())
			                     {
				                     if (EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
				                     {
					                     // 입찰 성공
					                     LOG_WARN(TEXT("Bid Success!"));
				                     }
				                     else
				                     {
					                     // 입찰 실패, InResponse->GetContentAsString() : 서버에서 알려준 입찰 실패 이유
					                     LOG_WARN(TEXT("bid Failed!"));
					                     // Json reader 생성
					                     UE_LOG(LogTemp, Warning, TEXT("%s"), *InResponse->GetContentAsString());
				                     }
			                     }
		                     }, requestBody);
	}
}

/**
 * ItemID로 물품에 대한 입찰 기록을 조회합니다. DB에서 입찰 순서대로 내림차순해서 정렬하여 주니, 그대로 배열을 사용하시면 됩니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수 
 * @param InItemId : 입찰 기록을 조회할 상품의 ID
 */
void UItemManager::RequestBidRecordByItemId(FCallbackRefArray<FBidRecord> InFunc, uint32 InItemId)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(DA_NETWORK(BidRecordAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // Json reader 생성
									 TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
									 TArray<TSharedPtr<FJsonValue>> jsonValues;
									 FJsonSerializer::Deserialize(reader, jsonValues);

									 TArray<FBidRecord> BidRecords;
									 for (TSharedPtr<FJsonValue>& bidInfo : jsonValues)
									 {
										 TSharedPtr<FJsonObject> bidInfoObj = bidInfo->AsObject();
										 BidRecords.Emplace(FBidRecord());
										 thisPtr->_JsonToData(bidInfoObj, *BidRecords.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(BidRecords);
									 }
								 	
									 LOG_N(TEXT("Get Bid Records Success!"));
								 }
							 });
	}
}

/**
 * ItemId로 물품을 삭제합니다. (JWT 토큰 확인으로 내 물품이 맞을 경우만 삭제함, 판매 종료 3시간 전에는 삭제 불가)
 * @param InItemId : 삭제할 상품의 ID
 */
void UItemManager::RequestRemoveItem(uint32 InItemId)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		httpHandler->Request(DA_NETWORK(RemoveItemAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::POST,[](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
								 	if (InbWasSuccessful && InResponse.IsValid())
								 	{
										 if (EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
										 {
											 // 삭제 성공
											 LOG_WARN(TEXT("Remove Success!"));
										 }
										 else
										 {
											 // 삭제 실패, InResponse->GetContentAsString() : 서버에서 알려준 삭제 실패 이유
											 LOG_WARN(TEXT("Remove Item Failed!"));
											 // Json reader 생성
											 UE_LOG(LogTemp, Warning, TEXT("%s"), *InResponse->GetContentAsString());
										 }
									 }
								 }
							 });
	}
}

/**
 * Type : Sell (내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청합니다.)
 * Type : Buy (내가 구매 성공한 물품을 요청합니다.)
 * id 순으로 내림차순 정렬이 되어, 최근에 업로드한 물품이 배열의 앞쪽에 등장합니다. (로그인 된 상태여야함)
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수
 * @param InMyItemReqType : 어떠한 내 아이템을 요청할 것인지 설정합니다. 
 */
void UItemManager::RequestMyItem(FCallbackRefArray<FItemData> InFunc, EMyItemReqType InMyItemReqType)
{
	FString reqAddURL;
	if(InMyItemReqType == EMyItemReqType::Buy)
		reqAddURL = DA_NETWORK(MyBuyItemAddURL);
	else if(InMyItemReqType == EMyItemReqType::Sell)
		reqAddURL = DA_NETWORK(MySellItemAddURL);
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(reqAddURL, EHttpRequestType::GET,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // Json reader 생성
									 TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
									 TArray<TSharedPtr<FJsonValue>> jsonValues;
									 FJsonSerializer::Deserialize(reader, jsonValues);

									 TArray<FItemData> itemDatas;
									 for (TSharedPtr<FJsonValue>& itemInfo : jsonValues)
									 {
										 TSharedPtr<FJsonObject> itemInfoObj = itemInfo->AsObject();
										 itemDatas.Emplace(FItemData());
										 thisPtr->_JsonToData(itemInfoObj, *itemDatas.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(itemDatas);
									 }
								 	
									 LOG_N(TEXT("Get My Items Success!"));
								 }
							 });
	}
}

/**
 * Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다. 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 등록할 상품의 ID
 * @param InItemLoc : 상품을 등록할 ItemActor의 위치
 */
void UItemManager::_Server_RegisterItemByLoc(uint32 InItemId, uint8 InItemLoc)
{
	CHECK_DEDI_FUNC;

	const uint8 registerIdx = InItemLoc - 1;
	
	if(!ItemActors.IsValidIndex(registerIdx))
	{
		LOG_WARN(TEXT("%d Item Location is %d! InValid Location. Can't register Item."), InItemId, InItemLoc);
		return;
	}
	
	if(ItemActors[registerIdx].IsValid())
	{
		ItemActors[registerIdx]->SetItemID(InItemId);
	}
}

/**
 * Json 형태의 ItemData String을 FItemData로 변환한다.
 * @param InJsonObj : Json 형태의 itemData String
 * @param OutItemData : 파싱한 정보를 담아갈 ItemData
 */
void UItemManager::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const
{
	InJsonObj->TryGetNumberField(TEXT("id"), OutItemData.ItemID);
	InJsonObj->TryGetStringField(TEXT("sellerUsername"), OutItemData.SellerName);
	InJsonObj->TryGetStringField(TEXT("buyerUsername"), OutItemData.BuyerName);
	InJsonObj->TryGetStringField(TEXT("title"), OutItemData.Title);
	InJsonObj->TryGetStringField(TEXT("itemInformation"), OutItemData.Information);

	FString location;
	InJsonObj->TryGetStringField(TEXT("location"), location);
	OutItemData.Location = FCString::Atoi(*location);

	InJsonObj->TryGetStringField(TEXT("world"), OutItemData.World);

	FString type;
	InJsonObj->TryGetStringField(TEXT("type"), type);
	if(type == TEXT("auction")) OutItemData.Type = EItemDealType::Auction;
	else if(type == TEXT("normal")) OutItemData.Type = EItemDealType::Normal;
	
	InJsonObj->TryGetNumberField(TEXT("imgCount"), OutItemData.ImgCount);
	InJsonObj->TryGetNumberField(TEXT("startPrice"), OutItemData.StartPrice);
	InJsonObj->TryGetNumberField(TEXT("currentPrice"), OutItemData.CurrentPrice);

	bool haveGlb = false;
	InJsonObj->TryGetBoolField(TEXT("haveGLB"), haveGlb);
	OutItemData.HaveGlb = haveGlb;

	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	InJsonObj->TryGetArrayField(TEXT("endTime"), out);

	for (const TSharedPtr<FJsonValue>& value : *out) // 파싱한다.
	{
		OutItemData.EndTime.Emplace(value->AsNumber());
	}
}

/**
 * Json 형태의 BidRecord JsonObject를 FBidRecord로 변환한다.
 * @param InJsonObj : Json 형태의 BidRecord String
 * @param OutBidRecord : 파싱한 정보를 담아갈 BidRecord
 */
void UItemManager::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FBidRecord& OutBidRecord) const
{
	InJsonObj->TryGetNumberField(TEXT("id"), OutBidRecord.Order);
	InJsonObj->TryGetNumberField(TEXT("bidPrice"), OutBidRecord.BidPrice);
	InJsonObj->TryGetStringField(TEXT("bidUser"), OutBidRecord.BidUser);
	
	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	InJsonObj->TryGetArrayField(TEXT("bidTime"), out);

	for (const TSharedPtr<FJsonValue>& value : *out) // 파싱한다.
	{
		OutBidRecord.BidTime.Emplace(value->AsNumber());
	}
}
