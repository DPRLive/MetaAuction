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
		// 위젯 숨기기
		SetVisibility(ESlateVisibility::Hidden);

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
	else
	{
		// 위젯 보이기
		SetVisibility(ESlateVisibility::Visible);

		// 입력 모드 변경 UI
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

		// 포커스 설정
		SetFocus();
	}
}