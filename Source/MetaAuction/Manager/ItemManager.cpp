
#include "ItemManager.h"
#include "../Actor/ItemActor.h"
#include "Data/ModelTransData.h"

#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>
#include <IStompMessage.h>

#include "Core/MAGameInstance.h"
#include "Data/LoginData.h"

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

		// 서버의 ItemManager에서는 Model들의 Trans를 관리한다.
		Server_ModelTransData = NewObject<UModelTransData>(this);
		Server_ModelTransData->LoadData();
		
		// TODO: 추후 login 기능 나오면 이후 로직으로 따로 빼면 좋을듯

		// 서버에서 WebSocket에 구독할 것들을 구독한다.
		if(const FStompHelper* stompHandler = MAGetStompHelper(GetOwner()->GetGameInstance()))
		{
			// 새 아이템 등록 알림 구독
			FStompSubscriptionEvent Server_EventNewItem;
			Server_EventNewItem.BindUObject(this, &UItemManager::_Server_OnNewItem);
			stompHandler->Subscribe(DA_NETWORK(WSNewItemAddURL), Server_EventNewItem);

			// 아이템의 삭제 / 종료 알림 구독
			FStompSubscriptionEvent Server_EventRemoveItem;
			Server_EventRemoveItem.BindUObject(this, &UItemManager::_Server_OnRemoveItem);
			stompHandler->Subscribe(DA_NETWORK(WSRemoveItemAddURL), Server_EventRemoveItem);
		}

		// 서버에서 ItemDataHandler에 아이템 정보 변경을 구독한다
		if(UItemDataHandler* itemDataHandler = MAGetItemDataHandler(GetWorld()->GetGameState()))
		{
			itemDataHandler->OnChangeItemData.AddUObject(this, &UItemManager::Server_ChangeItemData);
		}

		// 데디 서버를 키기 전 웹서버에 등록되어 있던 아이템들의 정보를 모두 가져온다.
		_Server_RegisterAllWorldItemID();
	}
}

/**
 * 상품이 삭제 되었을 때, 내 월드에 있었던 상품이면 지웁니다.
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_UnregisterItem(uint32 InItemId) const
{
	CHECK_DEDI_FUNC;
	
	// 선형 검색..
	for(const TWeakObjectPtr<AItemActor>& itemActor : ItemActors)
	{
		// 어 내 월드에 있던 물품이었네 -> 삭제
		if(itemActor.IsValid() && itemActor->GetItemID() == InItemId)
		{
			itemActor->Server_RemoveItem();

			// 트랜스폼 데이터도 지운다
			if(IsValid((Server_ModelTransData)))
			{
				Server_ModelTransData->RemoveTrans(InItemId);
			}
			break;
		}
	}
}

/**
 * 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
 * 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 쿼리할 상품의 ID
 */
void UItemManager::Server_RegisterNewItem(uint32 InItemId) const
{
	CHECK_DEDI_FUNC;
	
	if(const UItemDataHandler* itemDataHandler = MAGetItemDataHandler(GetWorld()->GetGameState()))
	{
		TWeakObjectPtr<const UItemManager> thisPtr = this;
		itemDataHandler->RequestItemDataById([thisPtr, InItemId](const FItemData& InItemData)
		{
			// TODO: 월드 관리 어떻게 해야하지.. 일단 하드코딩
			if (thisPtr.IsValid() && InItemData.World == TEXT("1"))
			{
				thisPtr->_Server_RegisterItemByLoc(InItemId, InItemData.Location);
			}
		}, InItemId);
	}
}

/**
 *  아이템 정보 변동을 처리한다.
 *  @param InItemId : 변경된 item ID
 *  @param InWorld : 그 물품이 있던 world
 *  @param InChangeList : 바뀐 항목에 대한 string ( -로 구분, (ex)location-world )
 */
void UItemManager::Server_ChangeItemData(const uint32& InItemId, const FString& InWorld, const FString& InChangeList) const
{
	CHECK_DEDI_FUNC;

	// 월드나 위치에 변동이 있었다면
	if(InChangeList.Contains(TEXT("world")) ||
		InChangeList.Contains(TEXT("location")))
	{
		Server_UnregisterItem(InItemId); // 삭제 후 재등록 시도해봄 (월드 1->1 같은 쿼리가 들어올 수도 있음)
		Server_RegisterNewItem(InItemId);
	}
	else if(InWorld == TEXT("1") && InChangeList.Contains(TEXT("glb"))) // 내 월드에 있는거고 위치는 안바뀌었는데 glb가 바뀌었다면
	{
		// TODO: 월드 확인 어쩌지??????
		// glb만 변경 된거면 클라이언트의 ItemManager에게 그거 redraw 명령
		// 배치된 곳을 찾는다
		for (uint8 idx = 0; idx < ItemActors.Num(); ++idx)
		{
			if (ItemActors[idx].IsValid() && (ItemActors[idx]->GetItemID() == InItemId))
			{
				_MulticastRPC_RedrawItem(idx);
				break;
			}
		}
	}
}

/**
 * 배치된 물품 모델링의 상대적 Transform을 변경합니다. (서버에서만 사용 가능)
 * @param InJwtToken : 검증을 위한 JwtToken
 * @param InItemLoc : 변경할 모델이 배치된 위치
 * @param InRelativeTrans : 변경할 모델의 상대적 Transform
 */
void UItemManager::Server_SetModelTransform(const FString& InJwtToken, const uint8 InItemLoc, const FTransform& InRelativeTrans)
{
	CHECK_DEDI_FUNC;
	
	const uint8 itemActorIdx = InItemLoc - 1;
	
	if(!ItemActors[itemActorIdx].IsValid())
		return;
	
	// 그 위치에 판매중인 물품을 확인
	const uint32 itemId = ItemActors[itemActorIdx]->GetItemID();

	// 검증 후 변경하던가 한다
	if(const UItemDataHandler* itemDataHandler = MAGetItemDataHandler(GetWorld()->GetGameState()))
	{
		TWeakObjectPtr<UItemManager> thisPtr = this;
		itemDataHandler->Server_ValidateItem(itemId, InJwtToken, [thisPtr, itemActorIdx, InRelativeTrans](const bool InIsMine)
		{
			if(thisPtr.IsValid() && InIsMine && thisPtr->ItemActors[itemActorIdx].IsValid())
			{
				// 검증 성공! 변경!
				thisPtr->ItemActors[itemActorIdx]->SetModelRelativeTrans(InRelativeTrans);

				const uint32 itemId = thisPtr->ItemActors[itemActorIdx]->GetItemID();
				// 정보도 저장
				thisPtr->Server_ModelTransData->TryEmplaceTrans(itemActorIdx + 1, itemId, InRelativeTrans);

				return;
			}

			LOG_WARN(TEXT("Transform edit failed!!"));
		});
	}
}

/**
 * 웹서버에 등록된 현재 월드에 배치되어 판매되고 있는 아이템들의 ID를 가져와 배치한다.
 * 데디 서버에서만 실행 가능합니다.
 */
void UItemManager::_Server_RegisterAllWorldItemID() const
{
	CHECK_DEDI_FUNC;
	
	if(const UItemDataHandler* itemDataHandler = MAGetItemDataHandler(GetWorld()->GetGameState()))
	{
		// 현재 월드에서, 구매 가능하게 배치되어 있는 상품을 검색하기 위한 Body를 만든다.
		// TODO : 월드 어케 관리함 근데? 일단 1로 하드코딩
		FItemSearchOption option;
		option.World = TEXT("1");
		option.CanDeal = EItemCanDeal::Possible;
		
		TWeakObjectPtr<const UItemManager> thisPtr = this;
		itemDataHandler->RequestItemDataByOption([thisPtr](const TArray<FItemData>& InItemData)
		{
			if (thisPtr.IsValid())
			{
				for (const FItemData& data : InItemData)
				{
					thisPtr->_Server_RegisterItemByLoc(data.ItemID, data.Location);
				}
			}
		}, option);
	}
}

/**
 * Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다. 데디 서버에서만 실행 가능합니다.
 * @param InItemId : 등록할 상품의 ID
 * @param InItemLoc : 상품을 등록할 ItemActor의 위치
 */
void UItemManager::_Server_RegisterItemByLoc(const uint32 InItemId, const uint8 InItemLoc) const
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
		ItemActors[registerIdx]->SetItemId(InItemId);
	}

	// 그릴 모델의 상대적 trans도 있다면 같이 설정해준다.
	if(IsValid(Server_ModelTransData))
	{
		if(auto* modelTrans = Server_ModelTransData->GetItemModelTrans().Find(InItemLoc))
		{
			if(modelTrans->Key == InItemId) // item id와 같으면 설정해줌
			{
				ItemActors[registerIdx]->SetModelRelativeTrans(modelTrans->Value);
				return;
			}
			// 아니면 들고 있을 필요가 없으니 지워버린다
			Server_ModelTransData->RemoveTrans(InItemLoc);
		}
	}
}

/**
 * Stomp를 통해 들어오는 새 아이템 등록 알림을 처리한다.
 */
void UItemManager::_Server_OnNewItem(const IStompMessage& InMessage) const
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
void UItemManager::_Server_OnRemoveItem(const IStompMessage& InMessage) const
{
	CHECK_DEDI_FUNC;

	uint32 itemID = FCString::Atoi(*InMessage.GetBodyAsString());

	LOG_N(TEXT("Server : Remove Item %d!"), itemID);
	
	// 아이템을 삭제한다.
	Server_UnregisterItem(itemID);
}


/**
 *  현재 월드에 있던 glb가 변경된 경우, 서버 -> 모든 클라에게 그 액터 다시 그리라고 명령합니다.
 *  @param InActorIdx : 다시 그릴 ItemActor의 Index
 */
void UItemManager::_MulticastRPC_RedrawItem_Implementation(const uint8& InActorIdx) const
{
	if(IsRunningDedicatedServer()) // 서버는 안해도 됩니다
		return;

	LOG_N(TEXT("Loc : %d, Redraw Item!"), InActorIdx + 1);
	
	if(ItemActors.IsValidIndex(InActorIdx) && ItemActors[InActorIdx].IsValid())
	{
		ItemActors[InActorIdx]->Client_RedrawModel();
	}
}