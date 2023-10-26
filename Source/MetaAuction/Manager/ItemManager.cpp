
#include "ItemManager.h"
#include "../Actor/ItemActor.h"
#include "Handler/ItemFileHandler.h"
#include "Core/MAGameInstance.h"

#include <Kismet/GameplayStatics.h>
#include <Interfaces/IHttpResponse.h>
#include <Serialization/JsonSerializer.h>
#include <Net/UnrealNetwork.h>
#include <IStompMessage.h>

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

		// 서버에서 WebSocket에 구독할 것들을 구독한다.
		if(FStompHandler* stompHandler = MAGetStompHandler(GetOwner()->GetGameInstance()))
		{
			// 새 아이템 등록 알림 구독
			FStompSubscriptionEvent Server_EventNewItem;
			Server_EventNewItem.BindUObject(this, &UItemManager::_Server_OnNewItem);
			stompHandler->Subscribe(DA_NETWORK(WSNewItemAddURL), Server_EventNewItem);

			// 아이템의 삭제 / 종료 알림 구독
			FStompSubscriptionEvent Server_EventRemoveItem;
			Server_EventRemoveItem.BindUObject(this, &UItemManager::_Server_OnRemoveItem);
			stompHandler->Subscribe(DA_NETWORK(WSRemoveItemAddURL), Server_EventRemoveItem);

			// 아이템 가격 변동 구독
			FStompSubscriptionEvent Server_EventChangePrice;
			Server_EventChangePrice.BindUObject(this, &UItemManager::_Server_OnChangePrice);
			stompHandler->Subscribe(DA_NETWORK(WSChangePriceAddURL), Server_EventChangePrice);

			// 아이템 정보 변동 구독
			FStompSubscriptionEvent Server_EventChangeItemData;
			Server_EventChangeItemData.BindUObject(this, &UItemManager::_Server_OnChangeItemData);
			stompHandler->Subscribe(DA_NETWORK(WSChangeDataAddURL), Server_EventChangeItemData);
			
		}
	}
}

/**
 * 상품이 삭제 되었을 때, 내 월드에 있었던 상품이면 지웁니다.
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
 * @param InFunc : 정보가 도착하면 실행할 람다 함수 
 * @param InItemId : 쿼리할 물품의 ID
 */
void UItemManager::RequestItemDataByID(FCallbackOneParam<const FItemData&> InFunc, uint32 InItemId)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
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
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
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

	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance())) // 로컬에 없으니 다운로드를 함
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
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
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
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
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
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
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
 * 현재 판매중인 물품이면서, 내가 입찰을 시도 했던 물품들을 보여줍니다. (로그인 된 상태여야함)
 * 최근에 입찰한게 배열의 앞쪽에 등장합니다.
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수
 */
void UItemManager::RequestMyBidItem(FCallbackRefArray<FItemData> InFunc)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetOwner()->GetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(DA_NETWORK(MyBidItemAddURL), EHttpRequestType::GET,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
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
								 	
									 LOG_N(TEXT("Get My Bid Items Success!"));
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
	if(out->Num() >= 5) // 년 월 일 시간 분 을 모두 파싱할 수 있을지
	{
		OutItemData.EndTime = FDateTime((*out)[0]->AsNumber(),
										(*out)[1]->AsNumber(),
											(*out)[2]->AsNumber(),
											(*out)[3]->AsNumber(),
											(*out)[4]->AsNumber());
	}

	const TArray<TSharedPtr<FJsonValue>>* out2; // 년 월 일 시간 분 초
	InJsonObj->TryGetArrayField(TEXT("lastModifiedTime"), out2);
	if (out2->Num() >= 6) // 년 월 일 시간 분 초를  모두 파싱할 수 있을지
	{
		OutItemData.LastModifyTime = FDateTime((*out2)[0]->AsNumber(),
		                                       (*out2)[1]->AsNumber(),
												   (*out2)[2]->AsNumber(),
											       (*out2)[3]->AsNumber(),
											       (*out2)[4]->AsNumber(),
											        (*out2)[5]->AsNumber());
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
	if(out->Num() >= 6) // 년 월 일 시간 분 초를 모두 파싱할 수 있을지
	{
		OutBidRecord.BidTime = FDateTime((*out)[0]->AsNumber(),
		                                 (*out)[1]->AsNumber(),
											(*out)[2]->AsNumber(),
											(*out)[3]->AsNumber(),
											(*out)[4]->AsNumber(),
											 (*out)[5]->AsNumber());
	}
}

/**
 * Stomp를 통해 들어오는 새 아이템 등록 알림을 처리한다.
 */
void UItemManager::_Server_OnNewItem(const IStompMessage& InMessage)
{
	CHECK_DEDI_FUNC;

	uint32 itemID = FCString::Atoi(*InMessage.GetBodyAsString());

	LOG_N(TEXT("Server : On New Item %d!"), itemID);
	
	// 새로운 아이템을 등록한다.
	Server_RegisterNewItem(itemID);
}

/**
 * Stomp를 통해 들어오는 아이템 제거 알림을 처리한다.
 */
void UItemManager::_Server_OnRemoveItem(const IStompMessage& InMessage)
{
	CHECK_DEDI_FUNC;

	uint32 itemID = FCString::Atoi(*InMessage.GetBodyAsString());

	LOG_N(TEXT("Server : Remove Item %d!"), itemID);
	
	// Item Manager에게 아이템 삭제를 요청한다.
	Server_UnregisterItem(itemID);
}

/**
 *  Stomp 메세지로 온 아이템 가격 변동 알림을 받는다.
 */
void UItemManager::_Server_OnChangePrice(const IStompMessage& InMessage) const
{
	CHECK_DEDI_FUNC;

	TArray<FString> idAndPrice;
	InMessage.GetBodyAsString().ParseIntoArray(idAndPrice, TEXT("-"), true);

	if(idAndPrice.Num() == 2)
	{
		_MulticastRPC_ChangePrice(FCString::Atoi(*idAndPrice[0]), FCString::Atoi64(*idAndPrice[1]));
	}
}

/**
 * 서버 -> RPC로 모든 클라에게 가격 변동 알림을 줍니다.
 * @param InItemId : 가격이 변동된 상품의 ID
 * @param InPrice : 현재 가격
 */
void UItemManager::_MulticastRPC_ChangePrice_Implementation(const uint32& InItemId, const uint64& InPrice) const
{
	LOG_WARN(TEXT("id[%d] change price : %lld"), InItemId, InPrice);

	if(!IsRunningDedicatedServer()) // 근데 서버는 안궁금해
		return;
	
	if(OnChangePrice.IsBound())
		OnChangePrice.Broadcast(InItemId, InPrice);
}

/**
 * 서버 -> RPC로 모든 클라에게 아이템 정보 변동 알림을 줍니다.
 * @param InItemId : 정보가 변동된 상품의 ID
 */
void UItemManager::_MulticastRPC_ChangeItemData_Implementation(const uint32& InItemId) const
{
	LOG_WARN(TEXT("id[%d] item data changed!"), InItemId);

	if(!IsRunningDedicatedServer()) // 근데 서버는 안궁금해
		return;

	if(OnChangeItemData.IsBound())
		OnChangeItemData.Broadcast(InItemId);
}

/**
 *  현재 월드에 있던 glb가 변경된 경우, 서버 -> 모든 클라에게 그 액터 다시 그리라고 명령합니다.
 *  @param InActorIdx : 다시 그릴 ItemActor의 Index
 */
void UItemManager::_MulticastRPC_RedrawItem_Implementation(const uint8& InActorIdx)
{
	if(IsRunningDedicatedServer()) // 서버는 안해도 됩니다
		return;

	if(ItemActors.IsValidIndex(InActorIdx) && ItemActors[InActorIdx].IsValid())
	{
		ItemActors[InActorIdx]->Client_RedrawModel();
	}
}

/**
 *  Stomp 메세지로 아이템 정보 변동 알림을 받는다.
 *  무조건 "(itemid)*(바뀌기전 월드)" 로 시작
 *  TODO : 할게 많네.
 */
void UItemManager::_Server_OnChangeItemData(const IStompMessage& InMessage)
{
	CHECK_DEDI_FUNC;

	TArray<FString> parseArr;
	InMessage.GetBodyAsString().ParseIntoArray(parseArr, TEXT("-"), true);

	if(parseArr.Num() <= 1) // 내용이 없으면 뭐야 이거
		return;

	// 쿼리의 첫 시작인 itemId, 바뀌기 전 월드 두개를 뽑아낸다.
	TArray<FString> idAndWorld;
	parseArr[0].ParseIntoArray(idAndWorld, TEXT("&"), true);
	
	if(idAndWorld.Num() < 2) // id와 world가 들어오지 않았다면 거부
		return;
	
	const uint32 itemId = FCString::Atoi(*idAndWorld[0]);
	const FString world = idAndWorld[1];
	
	// 현재 월드에 배치되어있던 glb에 변동이 있었는지 확인한다
	// TODO: 월드 확인 어쩌지??????
	if(world == TEXT("1")) // 내 월드에 있는건데 변동이 있었던 거면
	{
		if(InMessage.GetBodyAsString().Contains(TEXT("location"))) // 위치가 바뀌었음
		{
			// glb 상관 없이 아예 새로 등록한다
			Server_UnregisterItem(itemId);
			Server_RegisterNewItem(itemId);
		}
		else if(InMessage.GetBodyAsString().Contains(TEXT("glb"))) // 위치는 안바뀌었는데 glb가 바뀜
		{
			//glb만 변경 된거면 클라이언트의 ItemManager에게 그거 redraw 명령
			// 배치된 곳을 찾는다
			for(uint8 idx = 0; idx < ItemActors.Num(); ++idx)
			{
				if(ItemActors[idx].IsValid() && (ItemActors[idx]->GetItemID() == itemId))
				{
					_MulticastRPC_RedrawItem(idx);
					break;
				}
			}
		}
	}

	// 다른 속성 말고 딱 glb만 변경된거면 알림을 보내지 않는다
	if((parseArr.Num() == 2) && parseArr[1].Equals(TEXT("world"), ESearchCase::IgnoreCase))
	{
		return;
	}

	_MulticastRPC_ChangeItemData(itemId);
}