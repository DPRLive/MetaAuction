
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
 * 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_RegisterNewItem(uint32 InItemId)
{
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}

	TWeakObjectPtr<UItemManager> thisPtr = this;
	GetItemDataByID([thisPtr, InItemId](const FItemData& InItemData)
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
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}
	
	// 현재 월드에서, 구매 가능하게 배치되어 있는 상품을 검색하기 위한 Body를 만든다.
	// TODO : 월드 어케 관리함 근데? 일단 1로 하드코딩
	FItemSearchOption option;
	option.World = TEXT("1");
	option.CanDeal = EItemCanDeal::Possible;

	TWeakObjectPtr<UItemManager> thisPtr = this;
	GetItemDataByOption([thisPtr](const TArray<FItemData>& InItemData)
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
void UItemManager::GetItemDataByID(FGetItemDataByIdCallback InFunc, uint32 InItemId)
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
				thisPtr->_JsonToItemData(jsonObject, itemData);

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
 * @param InFunc : 정보가 도착하면 실행할 FGetItemDataByOptionCallback 형태의 함수 
 * @param InSearchOption : 물품 검색 시 사용할 옵션, 필요한 옵션만 설정 후 넣어서 사용하면 됩니다.
 */
void UItemManager::GetItemDataByOption(FGetItemDataByOptionCallback InFunc, const FItemSearchOption& InSearchOption)
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
										 thisPtr->_JsonToItemData(itemInfoObj, *itemDatas.rbegin());
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
* @param InItemID : 입찰할 item의 id
* @param InPrice : 입찰할 가격
*/
void UItemManager::Client_RequestBid(uint32 InItemID, uint64 InPrice)
{
	if (IsRunningDedicatedServer()) return; // 데디면 하지 마라.

	// Body를 만든다.
	FString requestBody = FString::Printf(TEXT("%llu"), InPrice);

	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		httpHandler->Request(DA_NETWORK(BidAddURL) + FString::Printf(TEXT("/%d"), InItemID), EHttpRequestType::POST,
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
 * Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다. 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 등록할 상품의 ID
 * @param InItemLoc : 상품을 등록할 ItemActor의 위치
 */
void UItemManager::_Server_RegisterItemByLoc(uint32 InItemId, uint8 InItemLoc)
{
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}

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
void UItemManager::_JsonToItemData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const
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