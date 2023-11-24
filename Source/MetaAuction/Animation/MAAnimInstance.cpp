// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MAAnimInstance.h"

#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAAnimInstance)

UMAAnimInstance::UMAAnimInstance()
{
	Owner = nullptr;
	MovingThreshold = 3.f;
	JumpingThreshold = 50.f;
	
	RSpeed = 0.f;
	FSpeed = 0.f;
	bIsIdle = false;
	bIsFalling = false;
	bIsJumping = false;
}

void UMAAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
}

void UMAAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(Owner.IsValid() && IsValid(Owner->GetCharacterMovement()))
	{
		RSpeed = FVector::DotProduct(Owner->GetVelocity(), Owner->GetActorRightVector());
		FSpeed = FVector::DotProduct(Owner->GetVelocity(), Owner->GetActorForwardVector());

		bIsIdle = Owner->GetVelocity().Size() < MovingThreshold;
		bIsFalling = Owner->GetCharacterMovement()->IsFalling();
		bIsJumping = bIsFalling & (Owner->GetVelocity().Z > JumpingThreshold);
	}
}
