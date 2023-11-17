// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// PCH 모음
#include <CoreMinimal.h>
#include <Engine/Engine.h>
#include <ProfilingDebugging/ScopedTimers.h>
#include <Interfaces/IHttpRequest.h>
#include <TimerManager.h>

#include "Common/MALog.h"
#include "Common/Enum.h"
#include "Util/UtilEnum.h"
#include "Util/UtilJson.h"
#include "Common/Typedef.h"
#include "DataAsset/MADataAssetHelper.h"
#include "Helper/HttpHelper.h"
#include "Helper/StompHelper.h"
#include "Handler/ItemFileHandler.h"
#include "Handler/ItemDataHandler.h"
#include "Handler/ChatHandler.h"

#define CHECK_DEDI_FUNC									\
	if(!IsRunningDedicatedServer())						\
	{													\
		LOG_WARN(TEXT("Client cannot run this func"));	\
		return;											\
	}

// 월드를 반환한다.
UWorld* MAGetWorld( UObject* InObject = nullptr );

// 게임 인스턴스를 반환한다.
UGameInstance* MAGetGameInstance( UWorld* InWorld = nullptr );

// 게임 스테이트를 반환한다.
AGameStateBase* MAGetGameState( UWorld* InWorld = nullptr );

// Http Helper 반환 
FHttpHelper* MAGetHttpHelper(UGameInstance* InGameInstance);

// ItemDataHanlder 반환
TObjectPtr<UItemDataHandler> MAGetItemDataHandler(AGameStateBase* InGameState);

// ItemFileHandler 반환
FItemFileHandler* MAGetItemFileHandler(UGameInstance* InGameInstance);

// ChatHandler 반환
TObjectPtr<UChatHandler> MAGetChatHandler(UGameInstance* InGameInstance);

// StompHelper 반환
FStompHelper* MAGetStompHelper(UGameInstance* InGameInstance);

// 현재 로그인된 UserName반환
FString MAGetMyUserName(UGameInstance* InGameInstance);

// 현재 로그인된 JwtToken 반환
FString MAGetMyJwtToken(UGameInstance* InGameInstance);