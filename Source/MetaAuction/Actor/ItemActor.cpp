// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "GameFramework/Pawn.h"
#include "UI/MAHUDWidget.h"
#include "Player/MAPlayerController.h"
#include "Component/ModelDrawComponent.h"

#include <Components/BoxComponent.h>
#include <Net/UnrealNetwork.h>

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

	ModelDrawComp = CreateDefaultSubobject<UModelDrawComponent>(TEXT("ModelDrawComp"));
	
	LevelPosition = 0;
	InteractInfo = FText();
	
	ItemID = 0;
	SellerName = TEXT("");
	ModelRelativeTrans = FTransform();
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemID);
	DOREPLIFETIME(AItemActor, SellerName);
	DOREPLIFETIME(AItemActor, ModelRelativeTrans);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
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
void AItemActor::Client_RedrawModel() const
{
	if(IsRunningDedicatedServer()) return; // 데디 ㄴㄴ
	
	// 모델을 지우고
	ModelDrawComp->RemoveModel();
	
	if (const FItemFileHandler* fileHandler = MAGetItemFileHandler(GetGameInstance()))
	{
		fileHandler->RemoveGlbFile(ItemID); // 파일을 지우고
	}
	
	// 다시 요청.
	_RequestCreateModel();
}

/**
 * 상호 작용을 위한 함수들
 */
bool AItemActor::CanInteracting_Implementation(AActor* InInteractorActor) const
{
	// 클라이언트가 조종하고 있는 특정 actor일때만 반응합니다.
	// 판매자랑 현재 로그인된 유저랑 일치할 때 반응합니다.
	if((InInteractorActor->GetLocalRole() != ROLE_AutonomousProxy) ||
		(SellerName != MAGetMyUserName(GetGameInstance())))
			return false;
	
	return true;
}

/**
 * 상호 작용시작 함수
 */
void AItemActor::BeginInteracting_Implementation(AActor* InInteractorActor, FHitResult& HitResult)
{
	// PlayerController를 가져올 수 있는지 확인합니다.
	AMAPlayerController* controller = nullptr;
	if(APawn* interActorPawn = Cast<APawn>(InInteractorActor))
	{
		controller = Cast<AMAPlayerController>(interActorPawn->GetController());
	}

	if(!IsValid(controller))
		return;

	// hud에게 어떤 상호작용인지 알려준다.
	UMAHUDWidget* hud = controller->GetHUDWidget();
	if(IsValid(hud))
	{
		hud->ToggleInteract(true, InteractInfo);
	}
}

/**
 * 상호 작용 종료 함수
 */
void AItemActor::EndInteracting_Implementation(AActor* InInteractorActor)
{
	// PlayerController를 가져올 수 있는지 확인합니다.
	AMAPlayerController* controller = nullptr;
	if(APawn* interActorPawn = Cast<APawn>(InInteractorActor))
	{
		controller = Cast<AMAPlayerController>(interActorPawn->GetController());
	}

	if(!IsValid(controller))
		return;

	// hud에게 상호작용 안내를 종료시킨다.
	UMAHUDWidget* hud = controller->GetHUDWidget();
	if(IsValid(hud))
	{
		hud->ToggleInteract(false);
	}
}

/**
 * 입력 상호 작용 함수
 */
void AItemActor::InputInteraction_Implementation(AActor* InteractorActor)
{
	LOG_N(TEXT("Input Interaction!"));

	// Player Controller에게 수정 위젯을 띄우라고 명령
	if(APawn* interActorPawn = Cast<APawn>(InteractorActor))
	{
		if(AMAPlayerController* controller = Cast<AMAPlayerController>(interActorPawn->GetController()))
		{
			controller->CreateModelTransEditWidget(LevelPosition, ModelRelativeTrans);
		}
	}
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

		// 모델을 지움
		ModelDrawComp->RemoveModel();
		ModelRelativeTrans = FTransform();
		return;
	}

	// 그린다.
	_RequestCreateModel();
}

/**
 * 배치된 모델의 상대 Transform이 Replicate 되면 호출
 */
void AItemActor::OnRep_ModelRelativeTrans() const
{
	if(!IsValid(ModelDrawComp))
		return;

	// world transform을 구해
	FTransform trans;
	trans.SetLocation(GetActorLocation() + ModelRelativeTrans.GetLocation());
	trans.SetRotation(GetActorQuat() * ModelRelativeTrans.GetRotation());
	trans.SetScale3D(ModelRelativeTrans.GetScale3D());
				
	// 트랜스폼을 변경한다.
	ModelDrawComp->SetModelTransform(trans);
}

/**
 * 모델을 그리기를 요청하는 함수입니다.
 */
void AItemActor::_RequestCreateModel() const
{
	FTransform trans;
	trans.SetLocation(GetActorLocation() + ModelRelativeTrans.GetLocation());
	trans.SetRotation(GetActorQuat() * ModelRelativeTrans.GetRotation());
	trans.SetScale3D(ModelRelativeTrans.GetScale3D());

	ModelDrawComp->CreateModel(ItemID, trans);
}
