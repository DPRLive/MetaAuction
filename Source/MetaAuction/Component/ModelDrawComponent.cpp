// Fill out your copyright notice in the Description page of Project Settings.


#include "ModelDrawComponent.h"

#include "glTFRuntimeAssetActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModelDrawComponent)

UModelDrawComponent::UModelDrawComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	
	ModelPtr = nullptr;
	ModelWorldTransform = FTransform();
}

void UModelDrawComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OnRequestModelCompleted.BindDynamic(this, &UModelDrawComponent::_DrawModel);
}

void UModelDrawComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * 모델 파일 경로를 기반으로 모델을 만들어 배치합니다.
 * @param InGlbPath : 만들 모델의 파일 경로
 * @param InWorldTransform : 만들 모델의 Transform
 */
void UModelDrawComponent::CreateModel(const FString& InGlbPath, const FTransform& InWorldTransform)
{
	ModelWorldTransform = InWorldTransform;
	
	UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InGlbPath, false, FglTFRuntimeConfig(), OnRequestModelCompleted);
}

/**
 * item id를 기반으로 모델을 만들어 배치합니다.
 * @param InItemId : 만들 모델의 item id
 * @param InWorldTransform : 만들 모델의 Transform
 */
void UModelDrawComponent::CreateModel(const uint32 InItemId, const FTransform& InWorldTransform)
{
	if (const FItemFileHandler* fileHandler = MAGetItemFileHandler(GetOwner()->GetGameInstance()))
	{
		// 파일 요청.
		TWeakObjectPtr<UModelDrawComponent> thisPtr = this;
		fileHandler->RequestGlb([thisPtr, InWorldTransform](const FString& InGlbPath)
		{
			if (thisPtr.IsValid())
			{
				thisPtr->CreateModel(InGlbPath, InWorldTransform);
			}
		}, InItemId);
	}
}

/**
 * 모델을 지웁니다. (Destroy)
 */
void UModelDrawComponent::RemoveModel()
{
	if(ModelPtr.IsValid())
		ModelPtr->Destroy();
	
	ModelPtr = nullptr;
	ModelWorldTransform = FTransform();
}

/**
 * 모델 파일의 Transform을 변경합니다.
 * @param InWorldTransform : 변경할 world transform
 */
void UModelDrawComponent::SetModelTransform(const FTransform& InWorldTransform)
{
	// 모델이 생성되어 있는 경우엔 변경 적용
	if(ModelPtr.IsValid())
	{
		ModelPtr->SetActorTransform(InWorldTransform);
	}

	// 아직 생성이 다 안된 경우를 위해 저장
	ModelWorldTransform = InWorldTransform;
}

/**
 * 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
 */
void UModelDrawComponent::_DrawModel(UglTFRuntimeAsset* InAsset)
{
	UE_SCOPED_TIMER(TEXT("Draw Model"), LogTemp, Warning);

	if (InAsset == nullptr)
	{
		LOG_ERROR(TEXT("UglTFRuntimeAsset is nullptr !"));
		return;
	}

	if (ModelPtr.IsValid()) // 이미 뭐가 그려져 있으면 지워버림.
	{
		ModelPtr->Destroy();
		ModelPtr = nullptr;
	}

	// UglTFRuntimeAsset을 기반으로 모델을 spawn 한다.
	ModelPtr = GetWorld()->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), ModelWorldTransform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (ModelPtr == nullptr)
	{
		LOG_ERROR(TEXT("Spawn AglTFRuntimeAssetActor Failed!"));
		return;
	}
	ModelPtr->Asset = InAsset;
	ModelPtr->FinishSpawning(ModelWorldTransform);
}