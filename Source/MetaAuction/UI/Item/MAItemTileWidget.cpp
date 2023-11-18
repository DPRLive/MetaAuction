// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Item/MAItemTileWidget.h"
#include "UI/Item/MAItemEntry.h"

#include <Components/TileView.h>
#include <Kismet/GameplayStatics.h>

UMAItemTileWidget::UMAItemTileWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemTileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemTileView);
}

void UMAItemTileWidget::UpdateSearchItems(const FItemSearchOption& InItemOption)
{
	ItemTileView->ClearListItems();

	UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState(GetWorld()));
	if (!IsValid(ItemDataHandler))
	{
		return;
	}

	LOG_SCREEN(FColor::Green, TEXT("Request %s"), *FString(__FUNCTION__));
	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto Func = [ThisPtr](const TArray<FItemData>& InData)
			{
				if (ThisPtr.IsValid())
				{
					ThisPtr->UpdateItems(InData);
					LOG_SCREEN(FColor::Green, TEXT("Successed %s"), *FString(__FUNCTION__));
				}
			};
		ItemDataHandler->RequestItemDataByOption(Func, InItemOption);
	}
}

void UMAItemTileWidget::UpdateMyItems(EMyItemReqType InType)
{
	ItemTileView->ClearListItems();

	UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState(GetWorld()));
	if (!IsValid(ItemDataHandler))
	{
		return;
	}

	LOG_SCREEN(FColor::Green, TEXT("Request %s"), *FString(__FUNCTION__));
	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto Func = [ThisPtr](const TArray<FItemData>& InData)
			{
				if (ThisPtr.IsValid())
				{
					ThisPtr->UpdateItems(InData);
					LOG_SCREEN(FColor::Green, TEXT("Successed %s"), *FString(__FUNCTION__));
				}
			};
		ItemDataHandler->RequestMyItem(Func, InType);
	}
}

void UMAItemTileWidget::UpdateItems(const TArray<FItemData>& ItemData)
{
	ItemTileView->ClearListItems();

	for (const FItemData& Item : ItemData)
	{
		UMAItemEntry* ItemEntry = NewObject<UMAItemEntry>();
		ItemEntry->ItemData = Item;
		ItemTileView->AddItem(ItemEntry);
	}
}
