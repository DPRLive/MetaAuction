// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatLogEntryWidget.h"
#include "UI/MAChatLogEntry.h"

#include <Components/TextBlock.h>

UMAChatLogEntryWidget::UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMAChatLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMAChatLogEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UMAChatLogEntry* Entry = Cast<UMAChatLogEntry>(ListItemObject);
	if (IsValid(Entry))
	{
		NameText->SetText(Entry->Data.ChatName);
		LogText->SetText(Entry->Data.ChatLog);
	}
}
