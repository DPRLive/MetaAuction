// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "glTFRuntimeFunctionLibrary.h"
#include "Interaction/MAInteractableInterface.h"

#include <GameFramework/Actor.h>
#include "ItemActor.generated.h"

class UBoxComponent;
class AglTFRuntimeAssetActor;

/**
 *  경매 물품을 랜더링해 줄 Actor,
 *  물품 정보는 ItemManager에게 따로 요청하여야 하며,
 *  이 Actor는 월드 배치용으로 월드에 배치 시 물품 ID를 가지고 있고 해당 물품 ID에 대한 모델링 렌더링을 담당합니다.
 */
UCLASS()
class METAAUCTION_API AItemActor : public AActor, public IMAInteractableInterface
{
	GENERATED_BODY()
   
public:
	AItemActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay */
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// 소멸자 용도로 쓸 BeginDestroy();
	virtual void BeginDestroy() override;

	// 물품을 설정한다.
	FORCEINLINE void SetItem(const uint32 InItemID, const FString& InSellerName) { ItemID = InItemID; SellerName = InSellerName;}
	
	// 모델의 상대적 Transform을 설정한다.
	FORCEINLINE void SetModelRelativeTrans(const FTransform& InTransform) { ModelRelativeTrans = InTransform; }
	
	// 현재 설정된 물품 id를 가져온다.
	FORCEINLINE uint32 GetItemID() const { return ItemID; }

	// 현재 설정된 물품의 판매자의 이름을 return 한다.
	FORCEINLINE const FString& GetSellerName() const { return SellerName; } 
	
	// 현재 레벨의 어떤 위치에 배치되어 있는지 알려줌
	FORCEINLINE const uint8 GetLevelPosition() const { return LevelPosition; }
	
	// 현재 이 액터에 배치된 물품을 지웁니다.
	// 서버에서만 호출 가능합니다.
	void Server_RemoveItem();

	// 이 액터에 배치된 모델링을 다시 다운받아 그립니다.
	// 클라이언트에서만 호출 가능합니다.
	void Client_RedrawModel();

	// 트랜스폼 수정 상호 작용을 위한 함수들
	virtual bool CanInteracting_Implementation(AActor* InInteractorActor) const override;
	virtual void BeginInteracting_Implementation(AActor* InInteractorActor, FHitResult& HitResult) override;
	virtual void EndInteracting_Implementation(AActor* InInteractorActor) override;
	virtual void InputInteraction_Implementation(AActor* InteractorActor) override;
	
private:
	// 물품 배치 해주는 함수, ItemID가 Replicate 되면 호출
	UFUNCTION()
	void OnRep_ItemID();

	// 배치된 모델의 상대 Transform이 Replicate 되면 호출
	UFUNCTION()
	void OnRep_ModelRelativeTrans();
	
	// 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
	// 클라이언트에서만 호출 가능합니다.
	UFUNCTION()
	void _Client_DrawModel(UglTFRuntimeAsset* InAsset);

private:
	// 액터의 RootComp입니다
	UPROPERTY( EditDefaultsOnly )
	TObjectPtr<UBoxComponent> RootComp;

	// 이 액터와 상호작용시 할 안내
	UPROPERTY( EditDefaultsOnly )
	FText InteractInfo;
	
	// 현재 이 ItemActor의 레벨상 위치, 레벨에 배치 후 설정 (1 부터 순서대로)
	// 웹과 공유해야하므로 평면도에 맞춰서 할당해주기
	UPROPERTY( EditAnywhere, meta = (AllowPrivateAccess = true) )
	uint8 LevelPosition;
	
	// 모델 요청이 완료되면 execute될 delegate
	FglTFRuntimeHttpResponse Client_OnRequestModelCompleted;
	
	// 이 액터에 배치된 물품 ID, 0은 할당되지 않음을 의미
	UPROPERTY( Transient, VisibleInstanceOnly, ReplicatedUsing = OnRep_ItemID )
	uint32 ItemID;

	// 이 액터에 배치된 물품의 판매자 이름
	UPROPERTY( Transient, VisibleInstanceOnly, Replicated )
	FString SellerName;
	
	// 모델링의 상대적 Transform
	UPROPERTY( Transient, VisibleInstanceOnly, ReplicatedUsing = OnRep_ModelRelativeTrans )
	FTransform ModelRelativeTrans;
	
	// 모델링된 Actor
	UPROPERTY( Transient, VisibleInstanceOnly )
	TWeakObjectPtr<AglTFRuntimeAssetActor> Client_Model;
};