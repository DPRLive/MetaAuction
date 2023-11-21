// Fill out your copyright notice in the Description page of Project Settings.


#include "MACharacterPlayer.h"
#include "Camera/MACameraModeComponent.h"
#include "Input/MAInputInitializeComponent.h"
#include "UI/MANameplateWidget.h"
#include "UI/Chat/MAChatBubbleWidgetComponent.h"
#include "Interaction/MAInteractorComponent.h"

#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/InputComponent.h>
#include <Components/WidgetComponent.h>
#include <Blueprint/UserWidget.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/SpringArmComponent.h>
#include <UObject/ConstructorHelpers.h>
#include <Kismet/GameplayStatics.h>


AMACharacterPlayer::AMACharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set character mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f).Quaternion());

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// CameraModeComponent
	CameraModeComponent = CreateDefaultSubobject<UMACameraModeComponent>(TEXT("CameraModeComponent"));

	// InputInitializeComponent
	InputInitializeComponent = CreateDefaultSubobject<UMAInputInitializeComponent>(TEXT("InputInitializeComponent"));

	// NameplateWidgetComponent
	NameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidgetComponent"));
	NameplateWidgetComponent->SetupAttachment(RootComponent);
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 15.0f));
	NameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameplateWidgetComponent->SetDrawSize(FVector2D(100.0f, 20.0f));
	NameplateWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// NameplateWidgetClass
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/MetaAuction/UI/WBP_Nameplate.WBP_Nameplate_C'"));
	if (WidgetClassRef.Succeeded())
	{
		NameplateWidgetClass = WidgetClassRef.Class;
	}

	// ChatBubbleWidgetComponent
	ChatBubbleWidgetComponent = CreateDefaultSubobject<UMAChatBubbleWidgetComponent>(TEXT("ChatBubbleWidgetComponent"));
	ChatBubbleWidgetComponent->SetupAttachment(RootComponent);
	ChatBubbleWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 40.0f));

	// InteractionComponent
	InteractorComponent = CreateDefaultSubobject<UMAInteractorComponent>(TEXT("InteractorComponent"));
	// 1인칭 & 3인칭 적절한 거리 찾으면 좋을듯
	InteractorComponent->InteractingRange = 1000.f;
}

/**
* 서버에서는 BeginPlay 이전에 Possession이 발생합니다.
* 클라이언트에서는 BeginPlay가 Possession보다 먼저 발생합니다.
*/
void AMACharacterPlayer::BeginPlay()
{
	LOG_WARN(TEXT("Begin!"));

	Super::BeginPlay();

	SetupNameplateWidget();
	LOG_WARN(TEXT("End!"));

}

void AMACharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupNameplateWidget();

	// 플레이어가 리슨 서버의 호스트인 경우 플레이어가 소유하기 전에 BeginPlay에서 위젯이 생성되었을 것이므로 숨깁니다.
	//if (IsLocallyControlled() && IsPlayerControlled() && NameplateWidgetComponent)
	//{
	//	NameplateWidgetComponent->SetTickMode(ETickMode::Disabled);
	//	NameplateWidgetComponent->SetVisibility(false, true);
	//}
}

void AMACharacterPlayer::OnRep_PlayerState()
{
	LOG_WARN(TEXT("Begin!"));

	Super::OnRep_PlayerState();
	LOG_WARN(TEXT("End!"));

}

void AMACharacterPlayer::SetupNameplateWidget()
{
	// AI나 서버의 PlayerController 사본이 아닌 로컬 소유 플레이어에 대해서만 UI를 설정합니다.
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController() && NameplateWidgetComponent)
	{
		if (UMANameplateWidget* NameplateWidget = CreateWidget<UMANameplateWidget>(PC, NameplateWidgetClass))
		{
			NameplateWidgetComponent->SetWidget(NameplateWidget);
			NameplateWidget->Update();
		}
	}
}
