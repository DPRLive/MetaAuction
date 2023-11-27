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
	Super::NativeDestruct();
}

void UMAChatRoomEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMAChatRoomEntry* Entry = Cast<UMAChatRoomEntry>(ListItemObject))
	{
		SellerNameText->SetText(FText::FromString(Entry->Data.Seller));
		UMAWidgetHelperLibrary::RequestImageByItemID(ItemImage, Entry->Data.ItemId);

		if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
		{
			TWeakObjectPtr<ThisClass> ThisPtr(this);

			// 채팅 정보를 모두 요청하여 마지막 메시지를 초기화합니다. (TODO : 비효율적이므로 추후 고쳐야 합니다.)
			auto RequestFunc = [ThisPtr](const TArray<FChatData>& Data)
				{
					if (ThisPtr.IsValid() && !Data.IsEmpty())
					{
						ThisPtr->LastMessageText->SetText(FText::FromString(Data.Last().Content));
						ThisPtr->LastTimeText->SetText(FText::FromString(Data.Last().Time.ToString()));
					}
				};
			ChatHandler->RequestChatsById(ERequestChatType::Chatroom, Entry->Data.ChatRoomId, RequestFunc);
		}
	}
}