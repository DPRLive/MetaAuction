// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatLogListWidget.h"

#include <Components/ListView.h>

UMAChatLogListWidget::UMAChatLogListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxEntryCount = 100;
}

void UMAChatLogListWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMAChatLogListWidget::AddChatLog(const FMAChatLogEntryData& InData)
{
	if (CurrentEntryCount >= MaxEntryCount)
	{
		UMAChatLogEntry* FirstEntry = nullptr;
		if (Entries.Dequeue(FirstEntry) && IsValid(FirstEntry))
		{
			ChatLogListView->RemoveItem(FirstEntry);
			CurrentEntryCount--;
		}
	}

	UMAChatLogEntry* Entry = NewObject<UMAChatLogEntry>(this);
	Entry->Data = InData;

	if (Entries.Enqueue(Entry))
	{
		ChatLogListView->AddItem(Entry);
		CurrentEntryCount++;
	}
}

void UMAChatLogListWidget::ClearChatLog()
{
	Entries.Empty();
	ChatLogListView->ClearListItems();
	CurrentEntryCount = 0;
}

void UMAChatLogListWidget::ScrollChatLogToBottom()
{
	ChatLogListView->ScrollToBottom();
}
