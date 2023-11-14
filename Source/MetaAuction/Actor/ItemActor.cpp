// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "glTFRuntimeAssetActor.h"
#include "GameFramework/Pawn.h"
#include "../UI/MANameplateWidget.h"

#include <Components/BoxComponent.h>
#include <Net/UnrealNetwork.h>
#include <Blueprint/UserWidget.h>


#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemActor)

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Visibility trace에만 반응 하도록 변경
	RootComp = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComp"));
	RootComp->SetBoxExtent(FVector(50.f, 50.f, 300.f));
	RootComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComp->SetGenerateOverlapEvents(false);
	RootComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComp->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(RootComp);
	
	LevelPosition = 0;
	ItemID = 0;
	SellerName = TEXT("");
	ModelRelativeTrans = FTransform();
	
	Client_Model = nullptr;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemID);
	DOREPLIFETIME(AItemActor, SellerName);
	DOREPLIFETIME(AItemActor, ModelRelativeTrans);
}

/**
 * Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay
 */
void AItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// BeginPlay 전 item id가 들어오는 경우(Replicate)가 있는거 같음, post init에서 bind
	Client_OnRequestModelCompleted.BindDynamic(this, &AItemActor::_Client_DrawModel);
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
	CHECK_DEDI_FUNC;
	ItemID = 0;
}

/**
 * 이 액터에 배치된 모델링을 다시 다운받아 그립니다.
 */
void AItemActor::Client_RedrawModel()
{
	if(IsRunningDedicatedServer()) return; // 데디 ㄴㄴ
	
	if (Client_Model.IsValid()) // 모델을 지우고
	{
		Client_Model->Destroy();
		Client_Model = nullptr;
	}
	
	if (FItemFileHandler* fileHandler = MAGetItemFileHandler(GetGameInstance()))
	{
		fileHandler->RemoveGlbFile(ItemID); // 파일을 지우고

		// 다시 요청.
		TWeakObjectPtr<AItemActor> thisPtr = this;
		fileHandler->RequestGlb([thisPtr](const FString& InGlbPath)
		{
			if (thisPtr.IsValid())
			{
				UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InGlbPath, false, FglTFRuntimeConfig(), thisPtr->Client_OnRequestModelCompleted);
			}
		}, ItemID);
	}
}

/**
 * 상호 작용을 위한 함수들
 */
bool AItemActor::CanInteracting_Implementation() const
{
	return true;
}

/**
 * 상호 작용시작 함수
 */
void AItemActor::BeginInteracting_Implementation(AActor* InteractorActor, FHitResult& HitResult)
{
	// 클라이언트가 조종하고 있는 특정 actor일때만 반응합니다.
	// 판매자랑 현재 로그인된 유저랑 일치할 때 반응합니다.
	if((InteractorActor->GetLocalRole() != ROLE_AutonomousProxy) ||
		(SellerName != MAGetMyUserName(GetGameInstance())))
		return;

	// PlayerController를 가져올 수 있는지 확인합니다.
	APlayerController* controller = nullptr;
	if(APawn* interActorPawn = Cast<APawn>(InteractorActor))
	{
		controller = Cast<APlayerController>(interActorPawn->GetController());
	}

	if(!IsValid(controller))
		return;

	// 위젯을 생성^^
	if (UMANameplateWidget* NameplateWidget = CreateWidget<UMANameplateWidget>(controller, GuideWidgetClass))
	{
		NameplateWidget->SetName(FText::FromString(TEXT("F 키를 눌러 Transform 수정")));
	
		FVector2D screenPos;
		controller->ProjectWorldLocationToScreen(HitResult.Location, screenPos);
		
		NameplateWidget->SetPositionInViewport(screenPos);
		NameplateWidget->AddToViewport(-1);
	}
	
	LOG_WARN(TEXT("Begin Interaction!"));

	
}

void AItemActor::EndInteracting_Implementation(AActor* InteractorActor)
{
	LOG_WARN(TEXT("End Interaction!"));
}

/**
 * 물품 배치 해주는 함수, ItemID가 Replicate 되면 호출
 * 서버에서는 OnRep_ 함수가 호출되지 않음
 */
void AItemActor::OnRep_ItemID()
{
	if (ItemID == 0)
	{
		LOG_N(TEXT("Item ID is Zero!"));

		if (Client_Model.IsValid()) // 모델이 있다면 지움
		{
			Client_Model->Destroy();
			Client_Model = nullptr;
		}
		
		ModelRelativeTrans = FTransform();
		return;
	}

	if (const FItemFileHandler* fileHandler = MAGetItemFileHandler(GetGameInstance()))
	{
		// 파일 요청.
		TWeakObjectPtr<AItemActor> thisPtr = this;
		fileHandler->RequestGlb([thisPtr](const FString& InGlbPath)
		{
			if (thisPtr.IsValid())
			{
				UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilenameAsync(InGlbPath, false, FglTFRuntimeConfig(), thisPtr->Client_OnRequestModelCompleted);
			}
		}, ItemID);
	}
}

/**
 * 배치된 모델의 상대 Transform이 Replicate 되면 호출
 */
void AItemActor::OnRep_ModelRelativeTrans()
{
	if(!Client_Model.IsValid())
		return;

	// 트랜스폼을 변경한다.
	Client_Model->SetActorLocation(GetActorLocation() + ModelRelativeTrans.GetLocation());
	Client_Model->SetActorRotation(GetActorRotation() + ModelRelativeTrans.GetRotation().Rotator());
	Client_Model->SetActorScale3D(ModelRelativeTrans.GetScale3D());
}

/**
 * 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
 */
void AItemActor::_Client_DrawModel(UglTFRuntimeAsset* InAsset)
{
	if(IsRunningDedicatedServer()) return; // 데디 ㄴㄴ
	
	UE_SCOPED_TIMER(TEXT("Draw Model"), LogTemp, Warning);

	if (InAsset == nullptr)
	{
		LOG_ERROR(TEXT("UglTFRuntimeAsset is nullptr !"));
		return;
	}

	if (Client_Model.IsValid()) // 이미 뭐가 그려져 있으면 지워버림.
	{
		Client_Model->Destroy();
		Client_Model = nullptr;
	}

	// UglTFRuntimeAsset을 기반으로 모델을 spawn 한다.
	Client_Model = GetWorld()->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(),GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Client_Model == nullptr)
	{
		LOG_ERROR(TEXT("Spawn AglTFRuntimeAssetActor Failed!"));
		return;
	}
	Client_Model->Asset = InAsset;
	Client_Model->FinishSpawning(GetActorTransform());

	// 트랜스폼 변경
	Client_Model->SetActorLocation(GetActorLocation() + ModelRelativeTrans.GetLocation());
	Client_Model->SetActorRotation(GetActorRotation() + ModelRelativeTrans.GetRotation().Rotator());
	Client_Model->SetActorScale3D(ModelRelativeTrans.GetScale3D());
}
