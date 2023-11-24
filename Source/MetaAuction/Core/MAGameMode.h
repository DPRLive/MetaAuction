// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameFramework/GameMode.h>
#include "MAGameMode.generated.h"

/**
 * Auction Map에서 사용할 게임모드입니다.
 */
UCLASS()
class METAAUCTION_API AMAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMAGameMode();

	// 레벨이 열리면 호출
	virtual void BeginPlay() override;

	// Transitions to WaitingToStart and calls BeginPlay on actors
	virtual void StartPlay() override;

	// 소멸자 용도로 쓸 BeginDestroy
	virtual void BeginDestroy() override;
};
