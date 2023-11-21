// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "MACharacter.h"
#include "MACharacterPlayer.generated.h"

class UMAInteractorComponent;
/**
 * 
 */
UCLASS()
class METAAUCTION_API AMACharacterPlayer : public AMACharacter
{
	GENERATED_BODY()
	
public:

	AMACharacterPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	// Server only
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
private:

	void SetupNameplateWidget();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UMACameraModeComponent> CameraModeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UMAInputInitializeComponent> InputInitializeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	TObjectPtr<class UWidgetComponent> NameplateWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<class UUserWidget> NameplateWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	TObjectPtr<class UMAChatBubbleWidgetComponent> ChatBubbleWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UMAInteractorComponent> InteractorComponent;
};
