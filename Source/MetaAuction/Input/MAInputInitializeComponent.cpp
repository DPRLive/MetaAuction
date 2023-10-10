// Fill out your copyright notice in the Description page of Project Settings.


#include "MAInputInitializeComponent.h"
#include "Character/MACharacter.h"
#include "Input/MAInputConfig.h"

#include <GameFramework/Character.h>
#include <GameFramework/PlayerController.h>
#include <Engine/LocalPlayer.h>
#include <InputMappingContext.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>

UMAInputInitializeComponent::UMAInputInitializeComponent()
{
	bReadyToBindInputs = false;
	bWantsInitializeComponent = true;
}

void UMAInputInitializeComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::InitializePlayerInput] PlayerInputComponent이 nullptr 입니다."));
		return;
	}

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::InitializePlayerInput] Pawn이 nullptr 입니다."));
		return;
	}

	if (InputConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::InitializePlayerInput] InputConfig가 nullptr 입니다."));
		return;
	}

	// Set up action bindings
	if (!bReadyToBindInputs)
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
		{

			EnhancedInputComponent->ClearActionBindings();

			// Jumping
			EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump);
			EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Completed, this, &ThisClass::Input_StopJumping);

			// Moving
			EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

			// Looking
			EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

			bReadyToBindInputs = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::InitializePlayerInput] UEnhancedInputComponent가 nullptr 입니다."));
		}
	}
	
	// Set up Input Mapping Context
	if (bReadyToBindInputs)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();

				if (InputMapping == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::BeginPlay] InputMapping가 nullptr 입니다."));
					return;
				}
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}
}

void UMAInputInitializeComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	AMACharacter* MACharacter = Cast<AMACharacter>(GetOwner());
	if (MACharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::InitializeComponent] MACharacter가 nullptr 입니다."));
		return;
	}

	MACharacter->SetupPlayerInputComponentDelegate.AddDynamic(this, &ThisClass::OnSetupPlayerInputComponent);
}

void UMAInputInitializeComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::BeginPlay] Pawn이 nullptr 입니다."));
		return;
	}
}

void UMAInputInitializeComponent::OnSetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Log, TEXT("[UMAInputInitializeComponent::OnSetupPlayerInputComponent]"));
	InitializePlayerInput(PlayerInputComponent);
}

void UMAInputInitializeComponent::Input_Move(const FInputActionValue& Value)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::Input_Look] Pawn이 nullptr 입니다."));
		return;
	}

	AController* Controller = Pawn->GetController();
	if (Controller != nullptr)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		Pawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void UMAInputInitializeComponent::Input_Look(const FInputActionValue& Value)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMAInputInitializeComponent::Input_Look] Pawn이 nullptr 입니다."));
		return;
	}

	AController* Controller = Pawn->GetController();
	if (Controller != nullptr)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		// add yaw and pitch input to controller
		Pawn->AddControllerYawInput(LookAxisVector.X);
		Pawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void UMAInputInitializeComponent::Input_Jump(const FInputActionValue& Value)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr)
	{
		Character->Jump();
	}
}

void UMAInputInitializeComponent::Input_StopJumping(const FInputActionValue& Value)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr)
	{
		Character->StopJumping();
	}
}
