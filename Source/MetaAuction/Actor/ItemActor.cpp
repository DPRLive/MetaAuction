// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "glTFRuntimeAssetActor.h"
#include "Handler/ItemFileHandler.h"
#include "Core/MAGameInstance.h"

#include <Net/UnrealNetwork.h>


#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemActor)


AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	LevelPosition = 0;
	ItemID = 0;
	ItemData = FItemData();
	RemainFileCnt = 0;

	Model = nullptr;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemID);
	DOREPLIFETIME(AItemActor, ItemData);
}

/**
 * Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay
 */
void AItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// BeginPlay 전 item id가 들어오는 경우(Replicate)가 있는거 같음, post init에서 bind
	OnRequestModelCompleted.BindDynamic(this, &AItemActor::_DrawModel);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * 소멸자 용도로 쓸 Begin Destroy
 */
void AItemActor::BeginDestroy()
{
	Super::BeginDestroy();
}

/**
 * 현재 이 액터에 배치된 물품을 지웁니다.
 * 서버에서만 호출 가능합니다.
 */
void AItemActor::Server_RemoveItem()
{
	if (!IsRunningDedicatedServer())
	{
		LOG_WARN(TEXT("Client cannot run this func"));
		return;
	}

	ItemID = 0;
	ItemData = FItemData();
}

/**
 * 이 액터에 배치된 모델링을 다시 다운받아 그립니다.
 */
void AItemActor::RedrawModel()
{
	if (Model.IsValid()) // 모델을 지우고
	{
		Model->Destroy();
		Model = nullptr;
	}

	// 사진 경로도 비우고
	ItemData.ImgPaths.Empty();
	
	if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
	{
		TWeakPtr<FItemFileHandler> handler = gameInstance->GetModelHandler();
		if (handler.IsValid())
		{
			handler.Pin()->RemoveItemFiles(ItemID); // 파일을 지우고

			// 다시 요청.
			ItemFileReqHandle = handler.Pin()->CompletedItemFileReq.AddUObject(this, &AItemActor::OnReqItemFileCompleted);
			handler.Pin()->RequestItemFiles(ItemID, ItemData.FileCount);
			RemainFileCnt = ItemData.FileCount;
		}
	}
}

/**
 * 물품 배치 해주는 함수, ItemID가 Replicate 되면 호출
 * 서버에서는 OnRep_ 함수가 호출되지 않음
 */
void AItemActor::OnRep_CreateItem()
{
	if (ItemID == 0)
	{
		LOG_N(TEXT("Item ID is Zero!"));

		if (Model.IsValid()) // 모델이 있다면 지움
		{
			Model->Destroy();
			Model = nullptr;
		}

		return;
	}

	// TODO : 이미 배치되었던 물품이 있다면, 그 물품의 모델링 데이터를 지운다. -> 이건 ItemManager에서 명령 해야할듯

	// 모델 파일을 요청한다.
	if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
	{
		TWeakPtr<FItemFileHandler> handler = gameInstance->GetModelHandler();
		if (handler.IsValid())
		{
			ItemFileReqHandle = handler.Pin()->CompletedItemFileReq.AddUObject(this, &AItemActor::OnReqItemFileCompleted);
			handler.Pin()->RequestItemFiles(ItemID, ItemData.FileCount);
			RemainFileCnt = ItemData.FileCount;
		}
	}
}

/**
 * item 관련 파일 요청이 끝나면 실행될 함수
 */
void AItemActor::OnReqItemFileCompleted(uint32 InItemID, const FString& InFilePath)
{
	// 내가 요청한 파일 아니면 무시
	if (InItemID != ItemID) { return; }

	if (FPaths::GetExtension(InFilePath).Equals(TEXT("glb"), ESearchCase::IgnoreCase)) // glb 파일이면 모델 요청
	{
		UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InFilePath, false, FglTFRuntimeConfig(), OnRequestModelCompleted);
	}
	else // 아니면 이미지 파일로, 경로 기억
	{
		ItemData.ImgPaths.Emplace(InFilePath);
	}

	// 다 다운되었으면 바인드 해제.
	if (--RemainFileCnt == 0)
	{
		if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
		{
			TWeakPtr<FItemFileHandler> handler = gameInstance->GetModelHandler();
			if (handler.IsValid())
			{
				handler.Pin()->CompletedItemFileReq.Remove(ItemFileReqHandle);
			}
		}
	}
}

/**
 * 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
 */
void AItemActor::_DrawModel(UglTFRuntimeAsset* InAsset)
{
	UE_SCOPED_TIMER(TEXT("Draw Model"), LogTemp, Warning);

	if (InAsset == nullptr)
	{
		LOG_ERROR(TEXT("UglTFRuntimeAsset is nullptr !"));
		return;
	}

	if (Model.IsValid()) // 이미 뭐가 그려져 있으면 지워버림.
	{
		Model->Destroy();
		Model = nullptr;
	}

	// UglTFRuntimeAsset을 기반으로 모델을 spawn 한다.
	Model = GetWorld()->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(),GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Model == nullptr)
	{
		LOG_ERROR(TEXT("Spawn AglTFRuntimeAssetActor Failed!"));
		return;
	}
	Model->Asset = InAsset;
	Model->FinishSpawning(GetActorTransform());
}
