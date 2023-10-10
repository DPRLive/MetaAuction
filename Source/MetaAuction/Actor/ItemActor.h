// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "glTFRuntimeFunctionLibrary.h"
#include <GameFramework/Actor.h>
#include "ItemActor.generated.h"

class AglTFRuntimeAssetActor;
/**
 *  현재 ItemActor에 할당된 경매 물품에 대한 정보를 저장
 */
USTRUCT()
struct FItemData
{
	GENERATED_BODY()

	FItemData() : SellerName(TEXT("")), BuyerName(TEXT("")), Title(TEXT("")), Information(TEXT("")),
	Location(0), FileCount(0), StartPrice(0), CurrentPrice(0) {}
	
	// 판매자 이름
	UPROPERTY(VisibleInstanceOnly)
	FString SellerName;

	// 구매자 이름
	UPROPERTY(VisibleInstanceOnly)
	FString BuyerName;
	
	// 물품 제목
	UPROPERTY(VisibleInstanceOnly)
	FString Title;

	// 물품 상세 정보
	UPROPERTY(VisibleInstanceOnly)
	FString Information;

	// 위치
	UPROPERTY(VisibleInstanceOnly)
	uint8 Location;
	
	// 파일의 갯수 (glb + 이미지 갯수 해서)
	UPROPERTY(VisibleInstanceOnly)
	uint32 FileCount;
	
	// 물품 경매 시작 가격
	UPROPERTY(VisibleInstanceOnly)
	uint64 StartPrice;

	// 물품 경매 현재 가격
	UPROPERTY(VisibleInstanceOnly)
	uint64 CurrentPrice;
	
	// 종료 시각. 배열에 순서대로 년 / 월 / 일 / 시간 / 분 
	UPROPERTY(VisibleInstanceOnly)
	TArray<uint16> EndTime;

	// 해당 물품의 이미지들의 경로, 없을 수도 있음, 절대 경로입니다.
	UPROPERTY(VisibleInstanceOnly)
	TArray<FString> ImgPaths;
};

/**
 *  경매 물품을 랜더링해 줄 Actor
 */
UCLASS()
class METAAUCTION_API AItemActor : public AActor
{
	GENERATED_BODY()
   
public:
	// 물품 정보(FItemData)에 대한 변경사항이 있을 시 알려주는 Delegate
	DECLARE_DELEGATE(FOnChangeItemData)
	FOnChangeItemData OnChangeItemData;
	
	AItemActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay */
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// 소멸자 용도로 쓸 BeginDestroy();
	virtual void BeginDestroy() override;

	// 물품 id를 설정한다.
	FORCEINLINE void SetItemID(uint32 InItemID) { ItemID = InItemID; }

	// 현재 설정된 물품 id를 가져온다.
	FORCEINLINE uint32 GetItemID() const { return ItemID; }

	// 물품 정보를 설정한다.
	FORCEINLINE void SetItemData(const FItemData& InItemData) { ItemData = InItemData; }

	// 물품 정보를 가져온다.
	FORCEINLINE const FItemData& GetItemData() const { return ItemData; }
	
	// 현재 레벨의 어떤 위치에 배치되어 있는지 알려줌
	FORCEINLINE const uint8 GetLevelPosition() const { return LevelPosition; }

	// 현재 이 액터에 배치된 물품을 지웁니다.
	void Server_RemoveItem();

	// 이 액터에 배치된 모델링을 다시 다운받아 그립니다.
	void RedrawModel();

private:
	// 물품 배치 해주는 함수, ItemID가 Replicate 되면 호출
	UFUNCTION()
	void OnRep_CreateItem();

	// item 관련 파일 요청이 끝나면 실행될 함수
	void OnReqItemFileCompleted(uint32 InItemID, const FString& InFilePath);
	
	// 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
	UFUNCTION()
	void _DrawModel(UglTFRuntimeAsset* InAsset);

	// 아이템에 대한 변경사항이 있을시 excute
	UFUNCTION()
	FORCEINLINE void OnRep_OnChangeItemData() const { OnChangeItemData.ExecuteIfBound(); }
	
	// 이 액터에 배치된 물품 ID, 0은 할당되지 않음을 의미
	UPROPERTY( VisibleInstanceOnly, ReplicatedUsing = OnRep_CreateItem)
	uint32 ItemID;
	
	// 현재 이 ItemActor에 배치된 Item에 대한 정보들
	UPROPERTY( VisibleInstanceOnly, ReplicatedUsing = OnRep_OnChangeItemData )
	FItemData ItemData;
	
	// 현재 이 ItemActor의 레벨상 위치, 레벨에 배치 후 설정 (1 부터 순서대로)
	// 웹과 공유해야하므로 평면도에 맞춰서 할당해주기
	UPROPERTY( EditAnywhere, meta = (AllowPrivateAccess = true) )
	uint8 LevelPosition;

	// 모델 요청이 완료되면 execute될 delegate
	FglTFRuntimeHttpResponse OnRequestModelCompleted;

	// 현재 필요한 파일들의 남은 개수
	uint32 RemainFileCnt;
	
	// 아이템 관련 파일들 요청이 완료되면 호출되는 delegate를 바인드했다 말았다 할때 쓸 handle
	FDelegateHandle ItemFileReqHandle;
	
	// 모델링된 Actor
	UPROPERTY( VisibleInstanceOnly )
	TWeakObjectPtr<AglTFRuntimeAssetActor> Model;
};