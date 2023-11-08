// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h> 
#include "ItemManager.generated.h"

class AItemActor;
class IStompMessage;

/**
 *  월드에 배치된 전체적인 물품들의 상태를 관리해주는 Item Manager
 *  GameState에서 관리하여 물품에 대한 관리는 server가 주도함.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UItemManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UItemManager();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// 상품이 삭제 되었을 때, 내 월드에 있었던 상품이면 지웁니다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_UnregisterItem(uint32 InItemId) const;
	
	// 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterNewItem(uint32 InItemId) const;
	
	// 아이템 정보 변동을 처리한다.
	void Server_ChangeItemData(const uint32& InItemId, const FString& InWorld, const FString& InChangeList) const;
	
private:
	// 웹서버에 등록된 현재 월드에 배치되어 판매되고 있는 아이템들의 ID를 가져와 배치한다.
	// 데디 서버에서만 실행 가능합니다.
	void _Server_RegisterAllWorldItemID() const;
	
	// Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다.
	// 데디 서버에서만 실행 가능합니다.
	void _Server_RegisterItemByLoc(uint32 InItemId, uint8 InItemLoc) const;

	// Stomp를 통해 들어오는 새 아이템 등록 알림을 처리한다.
	void _Server_OnNewItem(const IStompMessage& InMessage) const;

	// Stomp를 통해 들어오는 아이템 제거 알림을 처리한다.
	void _Server_OnRemoveItem(const IStompMessage& InMessage) const;
	
	// 현재 월드에 있던 glb가 변경된 경우, 서버 -> 모든 클라에게 그 액터 다시 그리라고 명령합니다.
	UFUNCTION( NetMulticast, Unreliable )
	void _MulticastRPC_RedrawItem(const uint8& InActorIdx) const;
	
	// 물품을 배치할 수 있는 ItemActor에 대한 포인터들. 사용자에게 표시는 1부터 할거지만 접근은 인덱스 0부터 해주세요.
	// 클라이언트들에서도 읽을 수 있습니다.
	// TODO : 근데 클라이언트에서 접근할 일이 있을까?
	UPROPERTY( Replicated )
	TArray<TWeakObjectPtr<AItemActor>> ItemActors;
};