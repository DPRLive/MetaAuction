// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemListWidget.h"
#include "UI/MAItemEntry.h"

#include <Components/ListView.h>
#include <Kismet/GameplayStatics.h>

UMAItemListWidget::UMAItemListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemListView);
}

void UMAItemListWidget::UpdateSearchItems(const FItemSearchOption& InItemOption)
{
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

void UMAItemListWidget::UpdateMyItems(EMyItemReqType InType)
{
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