// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAWidgetUtils.h"
#include "Core/MAGameInstance.h"
#include "Manager/ItemManager.h"
#include "Handler/ItemFileHandler.h"

#include <GameFramework/GameState.h>
#include <Kismet/GameplayStatics.h>

UItemManager* UMAWidgetUtils::GetItemManager(UWorld* World)
{
	if (!IsValid(World))
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : World is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (!IsValid(GameState))
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : GameState is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	UItemManager* ItemManager = GameState->GetComponentByClass<UItemManager>();
	if (!IsValid(ItemManager))
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : ItemManager is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}
	return ItemManager;
}

const TSharedPtr<FItemFileHandler> UMAWidgetUtils::GetItemFileHandler(UWorld* World)
{
	if (!IsValid(World))
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : World is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(World->GetGameInstance());
	if (!IsValid(MAGameInstance))
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : MAGameInstance is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	const TSharedPtr<FItemFileHandler>& ItemFileHandler = MAGameInstance->GetItemFileHandler();
	if (!ItemFileHandler.IsValid())
	{
		LOG_SCREEN(FColor::Red, TEXT("%s : ItemFileHandler is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	return ItemFileHandler;
}
