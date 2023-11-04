﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemAdditionalInfoWidget.h"
#include "UI/MAChatLogListWidget.h"
#include "MetaAuction.h"

#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

UMAItemAdditionalInfoWidget::UMAItemAdditionalInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommentCount = 0;
}

void UMAItemAdditionalInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(InformationText);
	ensure(SellerNameText);
	ensure(CommentText);
	ensure(InputText);
	ensure(InputButton);
	ensure(WBP_CommentList);

	if (IsValid(InputButton))
	{
		InputButton->OnClicked.AddDynamic(this, &ThisClass::InputButtonClicked);
	}
}

void UMAItemAdditionalInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();

	// 위젯이 제거되면 상품 댓글이 달릴 때 댓글 이벤트 언바인딩
	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (IsValid(ChatHandler) && ItemReplyHandle.IsValid())
	{
		ChatHandler->UnsubscribeItemReply(CachedItemData.ItemID, ItemReplyHandle);
	}
}

void UMAItemAdditionalInfoWidget::Update(const FItemData& InItemData)
{
	CachedItemData = InItemData;
	TitleText->SetText(FText::FromString(InItemData.Title));
	InformationText->SetText(FText::FromString(InItemData.Information));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));

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
	auto RequestFunc = [ThisPtr](const TArray<FItemReply>& Data)
		{
			if (ThisPtr.IsValid())
			{
				for (const FItemReply& ItemReply : Data)
				{
					ThisPtr->AddItemReplyToChatLog(ItemReply);
				}

				if (IsValid(ThisPtr->CommentText))
				{
					ThisPtr->CommentCount = Data.Num();
					ThisPtr->CommentText->SetText(FText::FromString(FString::Printf(TEXT("댓글 %d"), ThisPtr->CommentCount)));
				}
			}
		};
	ChatHandler->RequestItemReply(InItemData.ItemID, RequestFunc);
	
	// 상품 댓글이 달릴 때 댓글 이벤트 언바인딩
	if (ItemReplyHandle.IsValid())
	{
		ChatHandler->UnsubscribeItemReply(InItemData.ItemID, ItemReplyHandle);
	}

	// 상품 댓글이 달릴 때 댓글 이벤트 바인딩
	if (ItemReplyHandle.IsValid())
	{
		auto SubscribeFunc = [ThisPtr](const FItemReply& ItemReply)
			{
				if (ThisPtr.IsValid() && IsValid(ThisPtr->WBP_CommentList))
				{
					ThisPtr->AddItemReplyToChatLog(ItemReply);

					if (IsValid(ThisPtr->CommentText))
					{
						ThisPtr->CommentText->SetText(FText::FromString(FString::Printf(TEXT("댓글 %d"), ++ThisPtr->CommentCount)));
					}
				}
			};
		ItemReplyHandle = ChatHandler->SubscribeItemReply(InItemData.ItemID, SubscribeFunc);
	}
}

void UMAItemAdditionalInfoWidget::AddItemReplyToChatLog(const FItemReply& InItemReply)
{
	if (IsValid(WBP_CommentList))
	{
		FMAChatLogEntryData EntryData;
		EntryData.ChatName = FText::FromString(InItemReply.Sender);
		EntryData.ChatLog = FText::FromString(InItemReply.Content);
		EntryData.ChatTime = InItemReply.Time;
		WBP_CommentList->AddChatLog(EntryData);
	}
}

void UMAItemAdditionalInfoWidget::InputButtonClicked()
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
