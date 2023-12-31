﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "MACharacter.h"
#include "Interface/MAPerspectiveInterface.h"

#include "MACharacterPlayer.generated.h"

struct FStreamableHandle;
class UMAInteractorComponent;
/**
 * 
 */
UCLASS()
class METAAUCTION_API AMACharacterPlayer : public AMACharacter, public IMAPerspectiveInterface
{
	GENERATED_BODY()
	
public:

	AMACharacterPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	// Server only
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	// 시점을 변경합니다.
	virtual void ChangePerspective() override;

	// 메시가 로드되면 캐릭터의 메시를 설정합니다.
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

private:
	void SetupNameplateWidget();

	// PlayerState에 데이터가 도착했을때, 데이터를 받아내야 할 때 호출합니다.
	void _ReceiveUserData();

	// 메시가 로드되면 캐릭터의 메시를 설정합니다.
	void _MeshLoadCompleted();

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

private:
	// PlayerState로부터 데이터가 바뀔때 받아내기 위한 delegate handle입니다.
	FDelegateHandle ReceiveUserDataHandle;

	// 시점 변경을 위한 idx입니다.
	UPROPERTY(Transient)
	uint8 NowPerspectiveIdx;

	// 캐릭터 메시 비동기 로드를 위한 handle 입니다.
	TSharedPtr<FStreamableHandle> MeshHandle;
};
