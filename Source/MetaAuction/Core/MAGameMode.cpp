// Copyright Epic Games, Inc. All Rights Reserved.


#include "MAGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameMode)

AMAGameMode::AMAGameMode()
{
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
}

/**
 *  소멸자 용도로 사용할 Begin Destory
 */
void AMAGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}