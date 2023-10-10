// Fill out your copyright notice in the Description page of Project Settings.


#include "MACameraModeData.h"
#include "Character/MACharacter.h"

#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>

UMACameraModeData::UMACameraModeData()
{
	bInheritPitch = true;
	bInheritYaw = true;
	bInheritRoll = true;

	RelativeLocation = FVector(0.0f, 0.0f, 70.0f);
	TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	bSpringArmUsePawnControlRotation = true; // Rotate the arm based on the controller
	bCameraUsePawnControlRotation = false;

	bUseBlending = false;
	BlendTime = 0.5f;
	BlendFunction = EMACameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UMACameraModeData::GiveToData(AMACharacter* InCharacter)
{
	if (InCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UMACameraModeData::GiveToData] InCharacter가 nullptr 입니다."));
		return;
	}

	// Spring Arm
	USpringArmComponent* CameraBoom = InCharacter->FindComponentByClass<USpringArmComponent>();
	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = TargetArmLength;
		CameraBoom->SetRelativeLocation(RelativeLocation);
		CameraBoom->bUsePawnControlRotation = bSpringArmUsePawnControlRotation;
		CameraBoom->bInheritPitch = bInheritPitch;
		CameraBoom->bInheritYaw = bInheritYaw;
		CameraBoom->bInheritRoll = bInheritRoll;
	}

	// Camera
	UCameraComponent* FollowCamera = InCharacter->FindComponentByClass<UCameraComponent>();
	if (CameraBoom != nullptr)
	{
		FollowCamera->bUsePawnControlRotation = bCameraUsePawnControlRotation;
	}
}