// Fill out your copyright notice in the Description page of Project Settings.


#include "MACharacter.h"
#include "MACharacterDataSet.h"

AMACharacter::AMACharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupPlayerInputComponentDelegate.Broadcast(PlayerInputComponent);
}

void AMACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CharacterDataSet != nullptr)
	{
		CharacterDataSet->GiveToData(this);
	}
}