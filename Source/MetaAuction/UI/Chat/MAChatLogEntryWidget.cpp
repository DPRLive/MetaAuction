// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatLogEntryWidget.h"
#include "UI/Chat/MAChatLogEntry.h"

#include <Components/HorizontalBox.h>
#include <Components/Border.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/OverlaySlot.h>

UMAChatLogEntryWidget::UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMAChatLogEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

			if (UOverlaySlot* BoxSlot = Cast<UOverlaySlot>(Box->Slot))
			{
				BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
			}

			Border->SetBrushColor(OtherChatColor);
		}
		// 나 자신
		else
		{
			NameText->SetVisibility(ESlateVisibility::Collapsed);

			if (UOverlaySlot* BoxSlot = Cast<UOverlaySlot>(Box->Slot))
			{
				BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
			}

			Border->SetBrushColor(MyChatColor);
		}
		
		LogText->SetText(Entry->Data.ChatLog);
		TimeText->SetText(FText::FromString(Entry->Data.ChatTime.ToString()));
	}
}
