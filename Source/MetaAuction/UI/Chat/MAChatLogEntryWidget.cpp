// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatLogEntryWidget.h"
#include "UI/Chat/MAChatLogEntry.h"
#include "Core/MAPlayerState.h"

#include <Components/HorizontalBox.h>
#include <Components/Border.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/OverlaySlot.h>

UMAChatLogEntryWidget::UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseOtherChat = false;
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
		if(AMAPlayerState* MAPS = Cast<AMAPlayerState>(GetOwningPlayerState()))
		{
			if (bUseOtherChat && MAPS->GetUserData().UserName == Entry->Data.ChatName.ToString())
			{
				// 나 자신
				NameText->SetVisibility(ESlateVisibility::Collapsed);

				if (UOverlaySlot* BoxSlot = Cast<UOverlaySlot>(Box->Slot))
				{
					BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
				}

				Border->SetBrushColor(MyChatColor);
			}
			else
			{
				// 다른 사람
				NameText->SetText(FText::FromString(FString::Printf(TEXT("%s : "), *Entry->Data.ChatName.ToString())));
				NameText->SetVisibility(ESlateVisibility::Visible);

				if (UOverlaySlot* BoxSlot = Cast<UOverlaySlot>(Box->Slot))
				{
					BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
				}

				Border->SetBrushColor(OtherChatColor);
			}

			LogText->SetText(Entry->Data.ChatLog);
			TimeText->SetText(FText::FromString(Entry->Data.ChatTime.ToString()));
		}
	}
}
