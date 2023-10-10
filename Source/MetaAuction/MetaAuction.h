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
#include "Common/Typedef.h"
#include "DataAsset/MADataAssetHelper.h"
#include "Handler/HttpHandler.h"

// 월드를 반환한다.
UWorld* MAGetWorld( UObject* InObject = nullptr );

// 게임 인스턴스를 반환한다.
UGameInstance* MAGetGameInstance( UWorld* InWorld = nullptr );

// Http Handler 반환 
FHttpHandler* MAGetHttpHandler(UGameInstance* InGameInstance);