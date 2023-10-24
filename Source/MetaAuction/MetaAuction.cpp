// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaAuction.h"
#include "Core/MAGameInstance.h"

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
 * Http Handler 반환
 */
FHttpHandler* MAGetHttpHandler(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetHttpHandler().IsValid())
		{
			return gameInstance->GetHttpHandler().Get();
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
		if(gameInstance->GetHttpHandler().IsValid())
		{
			return gameInstance->GetItemFileHandler().Get();
		}
	}
	return nullptr;
}

/**
 * Stomp Handler 반환
 */
FStompHandler* MAGetStompHandler(UGameInstance* InGameInstance)
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(InGameInstance))
	{
		if(gameInstance->GetHttpHandler().IsValid())
		{
			return gameInstance->GetStompHandler().Get();
		}
	}
	return nullptr;
}