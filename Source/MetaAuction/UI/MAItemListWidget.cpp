// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemListWidget.h"

#include <Components/ListView.h>

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
