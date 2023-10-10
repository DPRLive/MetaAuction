
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
 * 상품 id로 webserver와 쿼리하여 정보를 가져온 후, 정보를 올바른 ItemActor에 등록한다.
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_RequestItemDataAndRegister(uint32 InItemId)
{
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		FString RequestUrl = DA_NETWORK(ItemInfoAddURL) + FString::Printf(TEXT("/%d"), InItemId);

		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(RequestUrl, EHttpRequestType::GET, [thisPtr, InItemId](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		{
			if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
			{
				// Json reader 생성
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
				TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
				FJsonSerializer::Deserialize(reader, jsonObject);

				FItemData itemData;
				thisPtr->_JsonToItemData(jsonObject, itemData);

				LOG_N(TEXT("Server : Get Item Data(item id : %d) Success!"), InItemId);
				
				// 상품을 등록한다.
				thisPtr->_Server_RegisterItemByLoc(InItemId, itemData);
			}
		});
	}
}


/**
 * 웹서버에 등록된 아이템들의 정보를 모두 가져와 배치한다.
 * 데디 서버에서만 실행 가능합니다.
 */
void UItemManager::Server_RegisterAllItem()
{
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<UItemManager> thisPtr = this;
		httpHandler->Request(DA_NETWORK(AllItemAddURL), EHttpRequestType::GET,[thisPtr](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // Json reader 생성
									 TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
									 TArray<TSharedPtr<FJsonValue>> jsonValues;
									 FJsonSerializer::Deserialize(reader, jsonValues);

									 for (TSharedPtr<FJsonValue>& itemInfo : jsonValues)
									 {
										 TSharedPtr<FJsonObject> itemInfoObj = itemInfo->AsObject();
										 uint32 itemID = itemInfoObj->GetNumberField(TEXT("id"));

										 FItemData itemData;
										 thisPtr->_JsonToItemData(itemInfoObj, itemData);
										 thisPtr->_Server_RegisterItemByLoc(itemID, itemData);
									 }

									 LOG_N(TEXT("Server : Get All Item Data And Register Success!"));
								 }
							 });
	}
}

/**
 * Item Data의 Location을 기준으로, 상품을 등록한다.
 * @param InItemId : 등록할 상품의 ID
 * @param InItemData : 등록할 상품의 data
 */
void UItemManager::_Server_RegisterItemByLoc(uint32 InItemId, const FItemData& InItemData)
{
	if(!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}

	uint8 registerIdx = InItemData.Location - 1;
	
	if(!ItemActors.IsValidIndex(registerIdx))
	{
		LOG_WARN(TEXT("%d Item Location is %d! InValid Location. Can't register Item."), InItemId, InItemData.Location);
		return;
	}
	
	if(ItemActors[registerIdx].IsValid())
	{
		ItemActors[registerIdx]->SetItemData(InItemData);
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
	InJsonObj->TryGetStringField(TEXT("sellerUsername"), OutItemData.SellerName);
	InJsonObj->TryGetStringField(TEXT("buyerUsername"), OutItemData.BuyerName);
	InJsonObj->TryGetStringField(TEXT("title"), OutItemData.Title);
	InJsonObj->TryGetStringField(TEXT("itemInformation"), OutItemData.Information);

	// TODO: 위치 표시가 어떻게 될지 바뀔 수도 있음
	FString location;
	InJsonObj->TryGetStringField(TEXT("location"), location);
	OutItemData.Location = FCString::Atoi(*location);

	InJsonObj->TryGetNumberField(TEXT("fileCount"), OutItemData.FileCount);
	InJsonObj->TryGetNumberField(TEXT("startPrice"), OutItemData.StartPrice);
	InJsonObj->TryGetNumberField(TEXT("currentPrice"), OutItemData.CurrentPrice);

	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	InJsonObj->TryGetArrayField(TEXT("endTime"), out);

	for (const TSharedPtr<FJsonValue>& value : *out) // 파싱한다.
	{
		OutItemData.EndTime.Emplace(value->AsNumber());
	}
}