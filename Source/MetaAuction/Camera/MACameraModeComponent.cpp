// Fill out your copyright notice in the Description page of Project Settings.


#include "MACameraModeComponent.h"
#include "MACameraModeData.h"
#include "Character/MACharacter.h"

#include <GameFramework/SpringArmComponent.h>

UMACameraModeComponent::UMACameraModeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

void UMACameraModeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMACameraModeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NextCameraModeData != nullptr)
	{
		if (BlendAlpha == 1.0f)
		{
			SetCameraMode(NextCameraModeData);
		}
		else
		{
			UpdateBlending(DeltaTime);
			ApplyBlending();
		}
	}
}

void UMACameraModeComponent::SetCameraMode(UMACameraModeData* InCameraModeData)
{
	if (InCameraModeData == nullptr)
	{
		return;
	}

	CameraModeData = InCameraModeData;
	CameraModeData->GiveToData(Cast<AMACharacter>(GetOwner()));

	NextCameraModeData = nullptr;
}

void UMACameraModeComponent::PushCameraMode(UMACameraModeData* InCameraModeData)
{
	if (InCameraModeData != nullptr)
	{
		if (InCameraModeData->bUseBlending)
		{
			NextCameraModeData = InCameraModeData;
			
			// Spring Arm
			USpringArmComponent* CameraBoom = GetOwner()->FindComponentByClass<USpringArmComponent>();
			if (CameraBoom != nullptr)
			{
				StartTargetArmLength = CameraBoom->TargetArmLength;
				StartTargetArmLocation = CameraBoom->GetRelativeLocation();
			}
			// TODO : 소켓에 부착시 계산 다르게 해야할 수도?
			SetBlendWeight(0.0f);
		}
		else
		{
			SetCameraMode(InCameraModeData);
		}
	}
}

void UMACameraModeComponent::SetBlendWeight(float Weight)
{
	if (NextCameraModeData == nullptr)
	{
		return;
	}

	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	float BlendExponent = NextCameraModeData->BlendExponent;
	EMACameraModeBlendFunction BlendFunction = NextCameraModeData->BlendFunction;

	// 블렌드 가중치를 직접 설정하므로 블렌드 함수를 고려하여 블렌드 알파를 계산해야 합니다.
	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	switch (BlendFunction)
	{
	case EMACameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case EMACameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EMACameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EMACameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UMACameraModeComponent::UpdateBlending(float DeltaTime)
{
	if (NextCameraModeData == nullptr)
	{
		return;
	}

	float BlendTime = NextCameraModeData->BlendTime;
	float BlendExponent = NextCameraModeData->BlendExponent;
	EMACameraModeBlendFunction BlendFunction = NextCameraModeData->BlendFunction;

	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

	switch (BlendFunction)
	{
	case EMACameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	case EMACameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EMACameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EMACameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UMACameraModeComponent::ApplyBlending()
{
	if (NextCameraModeData == nullptr)
	{
		return;
	}

	USpringArmComponent* CameraBoom = GetOwner()->FindComponentByClass<USpringArmComponent>();
	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = StartTargetArmLength + (NextCameraModeData->TargetArmLength - StartTargetArmLength) * BlendWeight;
		CameraBoom->SetRelativeLocation(StartTargetArmLocation + (NextCameraModeData->RelativeLocation - StartTargetArmLocation) * BlendWeight);
	}

	LOG_SCREEN(FColor::Red, TEXT("%f"), BlendAlpha);
}
