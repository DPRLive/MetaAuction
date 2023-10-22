// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemListWidget.h"
#include "UI/MAItemEntry.h"

#include <Components/ListView.h>
#include <GameFramework/GameState.h>
#include <Kismet/GameplayStatics.h>

UMAItemListWidget::UMAItemListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemListView = Cast<UListView>(GetWidgetFromName(TEXT("ItemListView")));
	ensure(ItemListView);
}

void UMAItemListWidget::UpdateSearchItems(const FItemSearchOption& InItemOption)
{
	UItemManager* ItemManager = GetItemManager();
	if (!IsValid(ItemManager))
	{
		return;
	}

	LOG_SCREEN(FColor::Green, TEXT("Request %s"), *FString(__FUNCTION__));
	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto OnComplete = [ThisPtr](const TArray<FItemData>& InData)
			{
				ThisPtr->UpdateItems(InData);
				LOG_SCREEN(FColor::Green, TEXT("Successed %s"), *FString(__FUNCTION__));
			};
		ItemManager->RequestItemDataByOption(OnComplete, InItemOption);
	}
}

void UMAItemListWidget::UpdateMyItems(EMyItemReqType InType)
{
	UItemManager* ItemManager = GetItemManager();
	if (!IsValid(ItemManager))
	{
		return;
	}

	LOG_SCREEN(FColor::Green, TEXT("Request %s"), *FString(__FUNCTION__));
	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto OnComplete = [ThisPtr](const TArray<FItemData>& InData)
			{
				ThisPtr->UpdateItems(InData);
				LOG_SCREEN(FColor::Green, TEXT("Successed %s"), *FString(__FUNCTION__));
			};
		ItemManager->RequestMyItem(OnComplete, InType);
	}
}

void UMAItemListWidget::UpdateItems(const TArray<FItemData>& ItemData)
{
	ItemListView->ClearListItems();

	for (const FItemData& Item : ItemData)
	{
		UMAItemEntry* ItemEntry = NewObject<UMAItemEntry>();
		ItemEntry->ItemData = Item;
		ItemListView->AddItem(ItemEntry);
	}
}

UItemManager* UMAItemListWidget::GetItemManager()
{
	UWorld* World = GetWorld();
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
