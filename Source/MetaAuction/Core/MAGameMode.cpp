// Copyright Epic Games, Inc. All Rights Reserved.


#include "MAGameMode.h"
#include "MAGameInstance.h"

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
 
 // 준비가 완료되면 데디 서버일 시 자동으로 로그인합니다.
 if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
 {
  LOG_WARN(TEXT("Is Running Dedicated Server! Auto Login!"));
  gameInstance->RequestLogin(TEXT("test"), TEXT("test"));
 }
}

/**
 *  소멸자 용도로 사용할 Begin Destory
 */
void AMAGameMode::BeginDestroy()
{
 Super::BeginDestroy();
}
