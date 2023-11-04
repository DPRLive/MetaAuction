// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatLogWidget.h"
#include "UI/MAChatLogListWidget.h"
#include "Player/MAPlayerController.h"
#include "Common/MALog.h"

#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/Image.h>

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

	ensure(WBP_ChatLogList);
	ensure(InputScrollBox);
	ensure(InputText);
	ensure(InputShade);

	if (IsValid(InputText))
	{
		InputText->OnTextCommitted.AddDynamic(this, &ThisClass::InputTextCommitted);
		InputText->OnTextChanged.AddDynamic(this, &ThisClass::InputTextChanged);
	}

	AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer());
	if (IsValid(MAPC))
	{
		MAPC->OnReceivedChatLog.AddDynamic(this, &ThisClass::ReceivedChatLog);
	}

	if (IsValid(InputShade))
	{
		EnableColor = InputShade->GetColorAndOpacity();
		DisableColor = EnableColor;
		DisableColor.A = EnableColor.A * 0.5f;
		InputShade->SetColorAndOpacity(DisableColor);
	}
}

void UMAChatLogWidget::EnableInputText()
{
	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;
	UWidget* InWidgetToFocus = this;
	bool bFlushInput = false;

	// �Է� ��� ���� UI
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

		// ���콺 Ŀ�� ���̱�
		PlayerController->SetShowMouseCursor(true);
	}

	// ��Ŀ�� ����
	InputText->SetFocus();

	// �� ����
	InputShade->SetColorAndOpacity(EnableColor);
}

void UMAChatLogWidget::DisableInputText()
{
	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;
	UWidget* InWidgetToFocus = this;
	bool bFlushInput = false;

	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		// �Է� ��� ���� Game
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		if (bFlushInput)
		{
			PlayerController->FlushPressedKeys();
		}

		// ���콺 Ŀ�� �����
		PlayerController->SetShowMouseCursor(false);
	}

	// �� ����
	InputShade->SetColorAndOpacity(DisableColor);
}

void UMAChatLogWidget::SendInputText()
{
	if (!InputText->GetText().IsEmpty())
	{
		// TODO : �÷��̾� �̸� �ٿ��� ������
		FMAChatLogEntryData ChatLog;
		ChatLog.ChatName = FText::FromString(GetOwningPlayerPawn()->GetName());
		ChatLog.ChatLog = InputText->GetText();
		InputText->SetText(FText());

		// �ڱ� �ڽ� Ŭ���̾�Ʈ���� �ٷ� ȣ��
		ReceivedChatLog(ChatLog);

		// �÷��̾� ��Ʈ�ѷ����� ServerRPC ��û
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