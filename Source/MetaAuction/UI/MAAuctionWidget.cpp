// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAAuctionWidget.h"
#include "UI/MAItemTileWidget.h"
#include "UI/MAItemFilterWidget.h"
#include "Common/MALog.h"

#include <Components/HorizontalBox.h>
#include <Components/Button.h>

UMAAuctionWidget::UMAAuctionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);

	PreviousButton = nullptr;
	CurrentButton = nullptr;
}

void UMAAuctionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(MenuBox);
	ensure(ItemSearchButton);
	ensure(ItemBidOnButton);
	ensure(ItemRegisteredButton);
	ensure(RegisterItemButton);
	ensure(TransactionHistoryButton);
	ensure(WBP_ItemFilter);
	ensure(WBP_ItemView);

	if (IsValid(MenuBox))
	{
		TArray<UWidget*> Widgets = MenuBox->GetAllChildren();
		for (UWidget* Widget : Widgets)
		{
			UButton* Button = Cast<UButton>(Widget);
			if(IsValid(Button))
			{
				AllMenuButtons.Emplace(Button);
			}
		}
	}

	if (IsValid(ItemSearchButton))
	{
		ItemSearchButton->OnClicked.AddDynamic(this, &ThisClass::ItemSearchButtonClicked);
	}

	if (IsValid(ItemBidOnButton))
	{
		ItemBidOnButton->OnClicked.AddDynamic(this, &ThisClass::ItemBidOnButtonClicked);
	}

	if (IsValid(ItemRegisteredButton))
	{
		ItemRegisteredButton->OnClicked.AddDynamic(this, &ThisClass::ItemRegisteredButtonClicked);
	}

	if (IsValid(RegisterItemButton))
	{
		RegisterItemButton->OnClicked.AddDynamic(this, &ThisClass::RegisterItemButtonClicked);
	}

	if (IsValid(TransactionHistoryButton))
	{
		TransactionHistoryButton->OnClicked.AddDynamic(this, &ThisClass::TransactionHistoryButtonClicked);
	}

	if (IsValid(WBP_ItemFilter))
	{
		WBP_ItemFilter->OnSearch.AddDynamic(this, &ThisClass::Search);
	}

	ItemSearchButtonClicked();
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

		NotifyHiddenWidget();
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

		PlayerController->SetShowMouseCursor(true);

		SetFocus();

		NotifyVisibleWidget();
	}
}

void UMAAuctionWidget::ItemSearchButtonClicked()
{
	if (MenuButtonClicked(ItemSearchButton))
	{
		WBP_ItemView->UpdateSearchItems(WBP_ItemFilter->GetCurrentOption());
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMAAuctionWidget::ItemBidOnButtonClicked()
{
	if (MenuButtonClicked(ItemBidOnButton))
	{
		WBP_ItemView->UpdateMyItems(EMyItemReqType::Buy);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::ItemRegisteredButtonClicked()
{
	if (MenuButtonClicked(ItemRegisteredButton))
	{
		WBP_ItemView->UpdateMyItems(EMyItemReqType::Sell);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::RegisterItemButtonClicked()
{
	if (MenuButtonClicked(RegisterItemButton))
	{
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::TransactionHistoryButtonClicked()
{
	if (MenuButtonClicked(TransactionHistoryButton))
	{
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UMAAuctionWidget::MenuButtonClicked(UButton* ClickedButton)
{
	if (CurrentButton == ClickedButton)
	{
		return false;
	}

	PreviousButton = CurrentButton;
	CurrentButton = ClickedButton;

	if (IsValid(PreviousButton))
	{
		PreviousButton->SetStyle(CurrentButton->GetStyle());
	}

	FButtonStyle NewStyle = CurrentButton->GetStyle();
	NewStyle.SetNormal(NewStyle.Pressed);
	CurrentButton->SetStyle(NewStyle);

	return true;
}

void UMAAuctionWidget::Search(const FItemSearchOption& InOption)
{
	WBP_ItemView->UpdateSearchItems(InOption);
}

void UMAAuctionWidget::NotifyVisibleWidget()
{
}

void UMAAuctionWidget::NotifyHiddenWidget()
{
}
