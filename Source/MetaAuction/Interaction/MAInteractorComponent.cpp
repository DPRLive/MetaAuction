// Fill out your copyright notice in the Description page of Project Settings.


#include "MAInteractorComponent.h"
#include "MAInteractableInterface.h"

#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>
#include <Kismet/KismetSystemLibrary.h>
#include <DrawDebugHelpers.h>

UMAInteractorComponent::UMAInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractingRange = 2000.0f;
}

void UMAInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UMAInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteracting();
}

void UMAInteractorComponent::InputInteraction()
{
	// 상호작용이 가능하면 input 상호작용 호출
	if (InteractingActor.IsValid() &&
		InteractingActor->GetClass()->ImplementsInterface(UMAInteractableInterface::StaticClass()))
	{
		IMAInteractableInterface::Execute_InputInteraction(InteractingActor.Get(), GetOwner());
	}
}

void UMAInteractorComponent::NotifyInteractingActorChanged(AActor* OldActor, AActor* NewActor, FHitResult& HitResult)
{
	if (IsValid(OldActor))
	{
		if (OldActor->GetClass()->ImplementsInterface(UMAInteractableInterface::StaticClass()))
		{
			IMAInteractableInterface::Execute_EndInteracting(OldActor, GetOwner());
		}
	}

	if (IsValid(NewActor))
	{
		if (NewActor->GetClass()->ImplementsInterface(UMAInteractableInterface::StaticClass()))
		{
			IMAInteractableInterface::Execute_BeginInteracting(NewActor, GetOwner(), HitResult);
		}
	}
}

void UMAInteractorComponent::UpdateInteracting()
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(GetWorld()) || !IsValid(OwnerActor))
	{
		return;
	}

	FVector AimStart;
	FVector AimEnd;
	FRotator AimRot;
	FHitResult AimHit;

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (IsValid(OwnerPawn) && IsValid(OwnerPawn->GetController()))
	{
		OwnerPawn->GetController()->GetPlayerViewPoint(AimStart, AimRot);
	}
	else
	{
		OwnerActor->GetActorEyesViewPoint(AimStart, AimRot);
	}
	
	TArray<AActor*> PlayerPawnAttachedActors;
	OwnerActor->GetAttachedActors(PlayerPawnAttachedActors);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(OwnerActor);
	CollisionQueryParams.AddIgnoredActors(PlayerPawnAttachedActors);
	bool bHit = GetWorld()->LineTraceSingleByChannel(AimHit, AimStart, AimStart + AimRot.Vector() * InteractingRange, ECC_Visibility, CollisionQueryParams);

#if UE_ENABLE_DEBUG_DRAWING
	if (bDebug)
	{
		FHitResult DebugHit;
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), AimStart, AimStart + AimRot.Vector() * InteractingRange, TraceTypeQuery1, false, PlayerPawnAttachedActors, EDrawDebugTrace::ForOneFrame, DebugHit, true);
	}
#endif

	AActor* OldActor = InteractingActor.Get();
	AActor* NewActor = bHit ? AimHit.GetActor() : nullptr;

	if (nullptr != NewActor)
	{
		if (!NewActor->GetClass()->ImplementsInterface(UMAInteractableInterface::StaticClass()) ||
			!IMAInteractableInterface::Execute_CanInteracting(NewActor, GetOwner()))
		{
			NewActor = nullptr;
		}
	}

	if (OldActor != NewActor)
	{
		InteractingActor = NewActor;
		NotifyInteractingActorChanged(OldActor, NewActor, AimHit);
	}
}