// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BidRecord/MABidRecordEntryWidget.h"
#include "UI/BidRecord/MABidRecordEntry.h"

#include "Components/TextBlock.h"

UMABidRecordEntryWidget::UMABidRecordEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMABidRecordEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMABidRecordEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMABidRecordEntry* Entry = Cast<UMABidRecordEntry>(ListItemObject))
	{
		OrderText->SetText(FText::AsNumber(Entry->Data.Order));
		BidPriceText->SetText(FText::AsNumber(Entry->Data.BidPrice));
		BidTimeText->SetText(FText::FromString(Entry->Data.BidTime.ToString()));
		BidUserText->SetText(FText::FromString(Entry->Data.BidUser));
	}
}
