// Copyright Epic Games, Inc. All Rights Reserved.


#include "MAGameMode.h"
#include "../Handler/StompHandler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameMode)

AMAGameMode::AMAGameMode()
{
	StompHandler = nullptr;
}

/**
 *  레벨이 열리면 호출
 */
void AMAGameMode::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  시작시 호출
 *  Transitions to WaitingToStart and calls BeginPlay on actors
 */
void AMAGameMode::StartPlay()
{
	Super::StartPlay();

	StompHandler = MakeShareable(new FStompHandler());
}

/**
 *  소멸자 용도로 사용할 Begin Destory
 */
void AMAGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}