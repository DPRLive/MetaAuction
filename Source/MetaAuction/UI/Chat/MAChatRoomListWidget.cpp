// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatRoomListWidget.h"
#include "UI/Chat/MAChatRoomEntry.h"

#include <Components/ListView.h>

UMAChatRoomListWidget::UMAChatRoomListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAChatRoomListWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMAChatRoomListWidget::Update()
{
	ChatRoomListView->ClearListItems();

	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (!IsValid(ChatHandler))
	{
		return;
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);

	// 채팅방 모두 요청하기
	auto RequestFunc = [ThisPtr](const TArray<FChatRoomData>& Data)
		{
			if (ThisPtr.IsValid())
			{
				for (const FChatRoomData& ChatRoomData : Data)
				{
					if (UMAChatRoomEntry* Entry = NewObject<UMAChatRoomEntry>())
					{
						Entry->Data = ChatRoomData;
						ThisPtr->ChatRoomListView->AddItem(Entry);
					}
				}
			}
		};
	ChatHandler->RequestMyChatRoom(RequestFunc);
}