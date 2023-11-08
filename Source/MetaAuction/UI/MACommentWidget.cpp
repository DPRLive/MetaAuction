// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MACommentWidget.h"
#include "UI/MAChatLogListWidget.h"

#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

UMACommentWidget::UMACommentWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMACommentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(CommentText);
	ensure(InputText);
	ensure(InputButton);
	ensure(WBP_CommentList);

	if (IsValid(InputButton))
	{
		InputButton->OnClicked.AddDynamic(this, &ThisClass::InputButtonClicked);
	}
}

void UMACommentWidget::NativeDestruct()
{
	Super::NativeDestruct();

	// 위젯이 제거되면 상품 댓글이 달릴 때 댓글 이벤트 언바인딩
	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (IsValid(ChatHandler) && ItemReplyHandle.IsValid())
	{
		ChatHandler->UnsubscribeItemReply(CachedItemData.ItemID, ItemReplyHandle);
	}
}

void UMACommentWidget::Update(const FItemData& InItemData)
{
	CachedItemData = InItemData;

	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (!IsValid(ChatHandler))
	{
		return;
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (!ThisPtr.IsValid())
	{
		return;
	}

	// 상품 댓글 모두 요청하기
	auto RequestFunc = [ThisPtr](const TArray<FChatData>& Data)
		{
			if (ThisPtr.IsValid())
			{
				for (const FChatData& ItemReply : Data)
				{
					ThisPtr->AddChatDataToChatLog(ItemReply);
				}

				if (IsValid(ThisPtr->CommentText))
				{
					ThisPtr->CommentCount = Data.Num();
					ThisPtr->CommentText->SetText(FText::FromString(FString::Printf(TEXT("댓글 %d"), ThisPtr->CommentCount)));
				}
			}
		};
	ChatHandler->RequestChatsById(ERequestChatType::ItemReply, InItemData.ItemID, RequestFunc);

	// 상품 댓글이 달릴 때 댓글 이벤트 언바인딩
	if (ItemReplyHandle.IsValid())
	{
		ChatHandler->UnsubscribeItemReply(InItemData.ItemID, ItemReplyHandle);
	}

	// 상품 댓글이 달릴 때 댓글 이벤트 바인딩
	auto SubscribeFunc = [ThisPtr](const FChatData& ItemReply)
		{
			if (ThisPtr.IsValid() && IsValid(ThisPtr->WBP_CommentList))
			{
				ThisPtr->AddChatDataToChatLog(ItemReply);

				if (IsValid(ThisPtr->CommentText))
				{
					ThisPtr->CommentText->SetText(FText::FromString(FString::Printf(TEXT("댓글 %d"), ++ThisPtr->CommentCount)));
				}
			}
		};
	ItemReplyHandle = ChatHandler->SubscribeItemReply(InItemData.ItemID, SubscribeFunc);
}

void UMACommentWidget::AddChatDataToChatLog(const FChatData& InChatData)
{
	if (IsValid(WBP_CommentList))
	{
		FMAChatLogEntryData EntryData;
		EntryData.ChatName = FText::FromString(InChatData.Sender);
		EntryData.ChatLog = FText::FromString(InChatData.Content);
		EntryData.ChatTime = InChatData.Time;
		WBP_CommentList->AddChatLog(EntryData);
	}
}

void UMACommentWidget::InputButtonClicked()
{
	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (!IsValid(ChatHandler))
	{
		return;
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (!ThisPtr.IsValid())
	{
		return;
	}

	if (!IsValid(InputText) || InputText->GetText().IsEmpty())
	{
		return;
	}

	ChatHandler->AddReplyToItem(CachedItemData.ItemID, InputText->GetText().ToString());
	LOG_N(TEXT("%s -> Content : [%s]"), *FString(__FUNCTION__), *InputText->GetText().ToString());
	InputText->SetText(FText::GetEmpty());
}
