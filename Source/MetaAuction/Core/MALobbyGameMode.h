// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameModeBase.h>
#include "MALobbyGameMode.generated.h"

/**
 * 로비 (Standalone 상태)에서 사용할 게임모드 입니다.
 */
UCLASS()
class METAAUCTION_API AMALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMALobbyGameMode();
	
	// Transitions to WaitingToStart and calls BeginPlay on actors
	virtual void StartPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Login")
	TSubclassOf<UUserWidget> LoginWidgetClass;
	
	UPROPERTY( Transient )
	TObjectPtr<UUserWidget> LoginWidgetPtr;
};
