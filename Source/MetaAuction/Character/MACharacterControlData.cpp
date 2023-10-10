// Fill out your copyright notice in the Description page of Project Settings.


#include "MACharacterControlData.h"
#include "Character/MACharacterControlData.h"
#include "Character/MACharacter.h"

#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>

UMACharacterControlData::UMACharacterControlData()
{
    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    bOrientRotationToMovement = true; // Character moves in the direction of input...	
    RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    JumpZVelocity = 700.f;
    AirControl = 0.35f;
    MaxWalkSpeed = 500.f;
    MinAnalogWalkSpeed = 20.f;
    BrakingDecelerationWalking = 2000.f;
}

void UMACharacterControlData::GiveToData(AMACharacter* InCharacter)
{
    if (InCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("[UMACharacterControlData::GiveToData] InCharacter가 nullptr 입니다."));
        return;
    }

    // Pawn
    InCharacter->bUseControllerRotationPitch = bUseControllerRotationPitch;
    InCharacter->bUseControllerRotationYaw = bUseControllerRotationYaw;
    InCharacter->bUseControllerRotationRoll = bUseControllerRotationRoll;

    // CharacterMovement
    InCharacter->GetCharacterMovement()->bOrientRotationToMovement = bOrientRotationToMovement;
    InCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
    InCharacter->GetCharacterMovement()->RotationRate = RotationRate;
    InCharacter->GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    InCharacter->GetCharacterMovement()->AirControl = AirControl;
    InCharacter->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    InCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = MinAnalogWalkSpeed;
    InCharacter->GetCharacterMovement()->BrakingDecelerationWalking = BrakingDecelerationWalking;
}