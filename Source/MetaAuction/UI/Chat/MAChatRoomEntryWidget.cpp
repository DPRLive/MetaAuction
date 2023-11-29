// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatRoomEntryWidget.h"
#include "UI/Chat/MAChatRoomEntry.h"
#include "UI/MAWidgetHelperLibrary.h"

#include <Components/TextBlock.h>

UMAChatRoomEntryWidget::UMAChatRoomEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAChatRoomEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMAChatRoomEntryWidget::NativeDestruct()
{
	// 채팅 기록이 변동될 때 델리게이트 언바인딩
	if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
	{
		ChatHandler->OnChatDelegate.Remove(OnChatDelegateHandle);
	}

	Super::NativeDestruct();
}

void UMAChatRoomEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMAChatRoomEntry* Entry = Cast<UMAChatRoomEntry>(ListItemObject))
	{
		TitleText->SetText(FText::FromString(Entry->Data.Title));
		SellerNameText->SetText(FText::FromString(Entry->Data.Seller));
		LastMessageText->SetText(FText::FromString(Entry->LastChatData.Content));
		LastTimeText->SetText(FText::FromString(Entry->LastChatData.Time.ToString()));
		UMAWidgetHelperLibrary::RequestImageByItemID(ItemImage, Entry->Data.ItemId);

		if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
		{
			TWeakObjectPtr<ThisClass> ThisPtr(this);

			// 채팅 기록이 변동될 때 델리게이트 바인딩
			ChatHandler->OnChatDelegate.Remove(OnChatDelegateHandle);
			OnChatDelegateHandle = ChatHandler->OnChatDelegate.AddLambda([ThisPtr](const int32 InChatroomId, const FChatData& InChatData)
				{
					if (ThisPtr.IsValid())
					{
						if (UMAChatRoomEntry* Entry = Cast<UMAChatRoomEntry>(ThisPtr->GetListItem()))
						{
							if (Entry->Data.ChatRoomId == InChatroomId)
							{
								ThisPtr->LastMessageText->SetText(FText::FromString(InChatData.Content));
								ThisPtr->LastTimeText->SetText(FText::FromString(InChatData.Time.ToString()));
							}
						}
					}
				});
		}
	}
}