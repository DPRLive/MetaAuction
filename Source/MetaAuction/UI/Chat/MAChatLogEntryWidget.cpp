// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatLogEntryWidget.h"
#include "UI/Chat/MAChatLogEntry.h"

#include <Components/TextBlock.h>

UMAChatLogEntryWidget::UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMAChatLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(NameText);
	ensure(LogText);
	ensure(TimeText);
}

void UMAChatLogEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UMAChatLogEntry* Entry = Cast<UMAChatLogEntry>(ListItemObject);
	if (IsValid(Entry))
	{
		NameText->SetText(Entry->Data.ChatName);
		LogText->SetText(Entry->Data.ChatLog);
		TimeText->SetText(FText::FromString(Entry->Data.ChatTime.ToString()));
	}
}