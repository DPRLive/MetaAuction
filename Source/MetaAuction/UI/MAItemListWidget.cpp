// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemListWidget.h"
#include "UI/MAItemEntry.h"
#include "UI/MAWidgetUtils.h"

#include <Components/ListView.h>
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
	UItemManager* ItemManager = UMAWidgetUtils::GetItemManager(GetWorld());
	if (!IsValid(ItemManager))
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
		ItemManager->RequestItemDataByOption(Func, InItemOption);
	}
}

void UMAItemListWidget::UpdateMyItems(EMyItemReqType InType)
{
	UItemManager* ItemManager = UMAWidgetUtils::GetItemManager(GetWorld());
	if (!IsValid(ItemManager))
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
		ItemManager->RequestMyItem(Func, InType);
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