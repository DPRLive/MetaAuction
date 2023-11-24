// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaAuction.h"
#include "Core/MAGameInstance.h"
#include "Core/MAGameState.h"
#include "Data/LoginData.h"
#include "Core/MAPlayerState.h"

#include <Kismet/GameplayStatics.h>
#include <Modules/ModuleManager.h>

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, MetaAuction, "MetaAuction");

/**
 * Native C++의 경우 사용할 수 있는 GetWorld()
 */
UWorld* MAGetWorld(UObject* InObject)
{
	if (InObject != nullptr)
		return InObject->GetWorld();

	FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!world)
	{
		LOG_SCREEN(FColor::Red, TEXT( "Fail Get World!!" ));
		return nullptr;
	}
	return world->World();
}

/**
 * GameInstance 반환
 */
UGameInstance* MAGetGameInstance(UWorld* InWorld)
{
	UWorld* world = InWorld ? InWorld : MAGetWorld();

	return UGameplayStatics::GetGameInstance(world);
}

/**
 * GameState 반환
 */
AGameStateBase* MAGetGameState(UWorld* InWorld)
{
	UWorld* world = InWorld ? InWorld : MAGetWorld();

	return UGameplayStatics::GetGameState(world);
}

/**
 * Http Helper 반환
 */
FHttpHelper* MAGetHttpHelper(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetHttpHelper().IsValid())
		{
			return gameInstance->GetHttpHelper().Get();
		}
	}
	return nullptr;
}

/**
 * Item Data Handler 반환
 */
TObjectPtr<UItemDataHandler> MAGetItemDataHandler(AGameStateBase* InGameState)
{
	if(AMAGameState* gameState = Cast<AMAGameState>(InGameState))
	{
		if(IsValid(gameState->GetItemDataHandler()))
		{
			return gameState->GetItemDataHandler();
		}
	}
	return nullptr;
}

/**
 * Item File Handler 반환
 */
FItemFileHandler* MAGetItemFileHandler(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetItemFileHandler().IsValid())
		{
			return gameInstance->GetItemFileHandler().Get();
		}
	}
	return nullptr;
}

/**
 * Chat Handler 반환
 */
TObjectPtr<UChatHandler> MAGetChatHandler(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(IsValid(gameInstance->GetChatHandler()))
		{
			return gameInstance->GetChatHandler();
		}
	}
	return nullptr;
}

/**
 * Stomp Helper 반환
 */
FStompHelper* MAGetStompHelper(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetStompHelper().IsValid())
		{
			return gameInstance->GetStompHelper().Get();
		}
	}
	return nullptr;
}

/**
 * 현재 로그인된 UserName반환
 */
FString MAGetMyUserName(UGameInstance* InGameInstance)
{
	// Player State에서 꺼내기 시도
	UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance);
	if(gameInstance == nullptr)
		return TEXT("");
	
	// Player State에서 꺼내온다.
	if(APlayerController* controller = gameInstance->GetFirstLocalPlayerController())
	{
		if(AMAPlayerState* playerState = controller->GetPlayerState<AMAPlayerState>())
		{
			return playerState->GetUserData().UserName;
		}
	}

	// 아니면 game instance에서 파싱해서 준다.
	if (gameInstance->GetFirstLocalPlayerController())
	{
		return gameInstance->GetLoginData()->GetMyUserName();
	}
	
	// 그것도 안되면 안줌
	return TEXT("");
}

/**
 * 현재 로그인된 JwtToken 반환
 */
FString MAGetMyJwtToken(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetLoginData().IsValid())
		{
			return gameInstance->GetLoginData()->GetJwtToken();
		}
	}
	return TEXT("");
}

/**
 * 현재 어떤 월드에 속해있는지 반환
 */
const FString MAGetNowWorldId(AGameStateBase* InGameState)
{
	if(AMAGameState* gameState = Cast<AMAGameState>(InGameState))
	{
		return gameState->GetWorldId();
	}
	
	return TEXT("");
}