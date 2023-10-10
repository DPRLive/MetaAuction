// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h> 
#include "ItemManager.generated.h"

class AItemActor;
class FJsonObject;
struct FItemData;
/**
 *  전체적인 물품들의 상태를 관리해주는 Item Manager
 *  GameState에서 관리하여 물품에 대한 관리는 server가 주도함.
 *  TODO: 물품 관리에 대한 쿼리용 함수 작성
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UItemManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemManager();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 상품 id로 webserver와 쿼리하여 정보를 가져온 후, 정보를 올바른 ItemActor에 등록한다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_RequestItemDataAndRegister(uint32 InItemId);

	// 웹서버에 등록된 아이템들의 정보를 모두 가져와 배치한다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterAllItem();

	// ItemActors에 대한 Getter입니다.
	FORCEINLINE const TArray<TWeakObjectPtr<AItemActor>>& GetItemActors() const { return ItemActors; }

protected:
	virtual void BeginPlay() override;

private:
	// Item Data의 Location을 기준으로, 상품을 등록한다. 데디 서버에서만 실행 가능합니다.
	void _Server_RegisterItemByLoc(uint32 InItemId, const FItemData& InItemData);
	
	// Json 형태의 ItemData JsonObejct를 FItemData로 변환한다.
	void _JsonToItemData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const;
	
	// 물품을 배치할 수 있는 ItemActor에 대한 포인터들. 사용자에게 표시는 1부터 할거지만 접근은 인덱스 0부터 해주세요.
	// 클라이언트들에서도 읽을 수 있습니다.
	UPROPERTY( Replicated )
	TArray<TWeakObjectPtr<AItemActor>> ItemActors;
};
