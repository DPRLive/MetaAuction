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

	Model = nullptr;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemID);
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
	
	if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
	{
		TWeakPtr<FItemFileHandler> handler = gameInstance->GetModelHandler();
		if (handler.IsValid())
		{
			handler.Pin()->RemoveGlbFile(ItemID); // 파일을 지우고

			// 다시 요청.
			TWeakObjectPtr<AItemActor> thisPtr = this;
			handler.Pin()->RequestGlb([thisPtr](const FString& InGlbPath)
			{
				if(thisPtr.IsValid())
				{
					UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InGlbPath, false, FglTFRuntimeConfig(), thisPtr->OnRequestModelCompleted);
				}
			}, ItemID);
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

	if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
	{
		TWeakPtr<FItemFileHandler> handler = gameInstance->GetModelHandler();
		if (handler.IsValid())
		{
			// 파일 요청.
			TWeakObjectPtr<AItemActor> thisPtr = this;
			handler.Pin()->RequestGlb([thisPtr](const FString& InGlbPath)
			{
				if(thisPtr.IsValid())
				{
					UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InGlbPath, false, FglTFRuntimeConfig(), thisPtr->OnRequestModelCompleted);
				}
			}, ItemID);
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
