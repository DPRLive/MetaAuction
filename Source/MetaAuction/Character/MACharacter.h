// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "GameFramework/Character.h"
#include "MACharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetupPlayerInputComponentSignature, UInputComponent*, PlayerInputComponent);

UCLASS()
class METAAUCTION_API AMACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AMACharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	//virtual void OnConstruction(const FTransform& Transform) override;

public:

	UPROPERTY(BlueprintAssignable, Category = Input)
	FSetupPlayerInputComponentSignature SetupPlayerInputComponentDelegate;

	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, Category = Data)
	TObjectPtr<class UMACharacterDataSet> CharacterDataSet;

};
