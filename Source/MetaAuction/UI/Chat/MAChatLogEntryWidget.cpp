// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatLogEntryWidget.h"
#include "UI/Chat/MAChatLogEntry.h"

#include <Components/HorizontalBox.h>
#include <Components/Border.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/ScrollBoxSlot.h>

UMAChatLogEntryWidget::UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMAChatLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(Box);
	ensure(Border);
	ensure(NameText);
	ensure(LogText);
	ensure(TimeText);

	OtherChatColor = Border->GetBrushColor();
}

void UMAChatLogEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (UMAChatLogEntry* Entry = Cast<UMAChatLogEntry>(ListItemObject))
	{
		// 다른 사람
		if (MAGetMyUserName(MAGetGameInstance()) != Entry->Data.ChatName.ToString())
		{
			NameText->SetText(FText::Format(FTextFormat(FText::FromString(TEXT("%s : "))), Entry->Data.ChatName));
			NameText->SetVisibility(ESlateVisibility::Visible);
			if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Box->Slot))
			{
				ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
			}

			Border->SetBrushColor(OtherChatColor);
		}
		// 나 자신
		else
		{
			NameText->SetVisibility(ESlateVisibility::Collapsed);
			if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Box->Slot))
			{
				ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
			}

			Border->SetBrushColor(MyChatColor);
		}
		
		LogText->SetText(Entry->Data.ChatLog);
		TimeText->SetText(FText::FromString(Entry->Data.ChatTime.ToString()));
	}
}
