// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAAuctionWidget.h"

UMAAuctionWidget::UMAAuctionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UMAAuctionWidget::ToggleWidget()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController))
	{
		return;
	}

	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;
	UWidget* InWidgetToFocus = this;
	bool bFlushInput = false;

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		// ���� �����
		SetVisibility(ESlateVisibility::Hidden);

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
	else
	{
		// ���� ���̱�
		SetVisibility(ESlateVisibility::Visible);

		// �Է� ��� ���� UI
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

		// ��Ŀ�� ����
		SetFocus();
	}
}