// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatRoomWidget.h"
#include "UI/Chat/MAChatLogListWidget.h"
#include "UI/MAWidgetHelperLibrary.h"
#include "Player/MAPlayerController.h"

#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

UMAChatRoomWidget::UMAChatRoomWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsInit = false;

	SetIsFocusable(true);
}

void UMAChatRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InputText->OnTextCommitted.AddDynamic(this, &ThisClass::InputTextCommitted);
	InputText->SetClearKeyboardFocusOnCommit(false);

	InputButton->OnClicked.AddDynamic(this, &ThisClass::SendInputText);
}

void UMAChatRoomWidget::NativeDestruct()
{
	// 델리게이트 언바인드
	if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
	{
		ChatHandler->OnChatDelegate.Remove(OnChatDelegateHandle);
	}

	Super::NativeDestruct();
}

void UMAChatRoomWidget::Update(const FChatRoomData& InChatRoomData)
{
	CachedChatRoomData = InChatRoomData;

	bIsInit = true;
	TitleText->SetText(FText::FromString(InChatRoomData.Title));

	// 상대방 이름 설정
	if(InChatRoomData.Seller == MAGetMyUserName(GetGameInstance()))
		SellerNameText->SetText(FText::FromString(InChatRoomData.Buyer));
	else
		SellerNameText->SetText(FText::FromString(InChatRoomData.Seller));
	
	PriceText->SetText(FText::AsNumber(InChatRoomData.Price));

	UMAWidgetHelperLibrary::RequestImageByItemID(ItemImage, InChatRoomData.ItemId);

	if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
	{
		WBP_ChatLogList->ClearChatLog();

		// 채팅 정보를 모두 요청하여 채팅 초기화
		TWeakObjectPtr<ThisClass> ThisPtr(this);
		auto Func = [ThisPtr](const TArray<FChatData>& Data)
			{
				if (ThisPtr.IsValid())
				{
					for (const FChatData& ChatData : Data)
					{
						ThisPtr->ReceivedChatLog(ChatData);
					}
				}
			};
		ChatHandler->RequestChatsById(ERequestChatType::Chatroom, InChatRoomData.ChatRoomId, Func);

		// 채팅 기록이 변동될 때 델리게이트 바인딩
		ChatHandler->OnChatDelegate.Remove(OnChatDelegateHandle);
		OnChatDelegateHandle = ChatHandler->OnChatDelegate.AddLambda([ThisPtr](const int32 InChatroomId, const FChatData& InChatData)
			{
				if (ThisPtr.IsValid() && ThisPtr->CachedChatRoomData.ChatRoomId == InChatroomId)
				{
					ThisPtr->ReceivedChatLog(InChatData);
				}
			});
	}
}

void UMAChatRoomWidget::SetFocusInputText()
{
	InputText->SetFocus();
}

void UMAChatRoomWidget::SendInputText()
{
	if (!InputText->GetText().IsEmpty() && bIsInit)
	{
		FString ChatLog = InputText->GetText().ToString();
		InputText->SetText(FText());

		if (UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance()))
		{
			ChatHandler->Send1On1Chat(CachedChatRoomData, ChatLog);
		}
	}
}

void UMAChatRoomWidget::InputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendInputText();
	}
}

void UMAChatRoomWidget::InputTextChanged(const FText& Text)
{
	InputScrollBox->ScrollToEnd();
}

void UMAChatRoomWidget::ReceivedChatLog(const FChatData& InData)
{
	FMAChatLogEntryData ChatLog;
	ChatLog.ChatName = FText::FromString(InData.Sender);
	ChatLog.ChatLog = FText::FromString(InData.Content);
	ChatLog.ChatTime = InData.Time;
	WBP_ChatLogList->AddChatLog(ChatLog);
	WBP_ChatLogList->ScrollChatLogToBottom();
}