// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatLogWidget.h"
#include "UI/MAChatLogListWidget.h"
#include "Player/MAPlayerController.h"

#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Components/EditableText.h>

UMAChatLogWidget::UMAChatLogWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
	bIsFocus = false;

	CurrentLogTextLine = 0;
	MaxLogTextLine = 100;
}

void UMAChatLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(InputText))
	{
		InputText->OnTextCommitted.AddDynamic(this, &ThisClass::InputTextCommitted);
		InputText->OnTextChanged.AddDynamic(this, &ThisClass::InputTextChanged);
	}

	AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer());
	if (IsValid(MAPC))
	{
		MAPC->OnReceivedChatLog.AddDynamic(this, &ThisClass::ReceivedChatLog);
	}
}

void UMAChatLogWidget::EnableInputText()
{
	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;
	UWidget* InWidgetToFocus = this;
	bool bFlushInput = false;

	// 입력 모드 변경 UI
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(InMouseLockMode);

		if (InWidgetToFocus != nullptr)
		{
			InputMode.SetWidgetToFocus(InWidgetToFocus->TakeWidget());
		}
		PlayerController->SetInputMode(InputMode);

		if (bFlushInput)
		{
			PlayerController->FlushPressedKeys();
		}

		// 마우스 커서 보이기
		PlayerController->SetShowMouseCursor(true);
	}

	// 포커스 설정
	InputText->SetFocus();
}

void UMAChatLogWidget::DisableInputText()
{
	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;
	UWidget* InWidgetToFocus = this;
	bool bFlushInput = false;

	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		// 입력 모드 변경 Game
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		if (bFlushInput)
		{
			PlayerController->FlushPressedKeys();
		}

		// 마우스 커서 숨기기
		PlayerController->SetShowMouseCursor(false);
	}
}

void UMAChatLogWidget::SendInputText()
{
	if (!InputText->GetText().IsEmpty())
	{
		// TODO : 플레이어 이름 붙여서 보내기
		FMAChatLogEntryData ChatLog;
		ChatLog.ChatName = FText::FromString(GetOwningPlayerPawn()->GetName());
		ChatLog.ChatLog = InputText->GetText();
		InputText->SetText(FText());

		// 자기 자신 클라이언트에는 바로 호출
		ReceivedChatLog(ChatLog);

		// 플레이어 컨트롤러에서 ServerRPC 요청
		AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer());
		if (IsValid(MAPC))
		{
			MAPC->SendChatLog(ChatLog);
		}
	}
}

void UMAChatLogWidget::InputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendInputText();
	}
	else
	{
		DisableInputText();
	}
}

void UMAChatLogWidget::InputTextChanged(const FText& Text)
{
	InputScrollBox->ScrollToEnd();
}

void UMAChatLogWidget::ReceivedChatLog(const FMAChatLogEntryData& InData)
{
	WBP_ChatLogList->AddChatLog(InData);
	WBP_ChatLogList->ScrollChatLogToBottom();
}
