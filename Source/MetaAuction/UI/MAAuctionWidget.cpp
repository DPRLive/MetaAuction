// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAAuctionWidget.h"
#include "UI/MAItemListWidget.h"
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

	ensure(ItemSearchButton);
	if (IsValid(ItemSearchButton))
	{
		ItemSearchButton->OnClicked.AddDynamic(this, &ThisClass::ItemSearchButtonClicked);
	}

	ensure(ItemBidOnButton);
	if (IsValid(ItemBidOnButton))
	{
		ItemBidOnButton->OnClicked.AddDynamic(this, &ThisClass::ItemBidOnButtonClicked);
	}

	ensure(ItemRegisteredButton);
	if (IsValid(ItemRegisteredButton))
	{
		ItemRegisteredButton->OnClicked.AddDynamic(this, &ThisClass::ItemRegisteredButtonClicked);
	}

	ensure(RegisterItemButton);
	if (IsValid(RegisterItemButton))
	{
		RegisterItemButton->OnClicked.AddDynamic(this, &ThisClass::RegisterItemButtonClicked);
	}

	ensure(TransactionHistoryButton);
	if (IsValid(TransactionHistoryButton))
	{
		TransactionHistoryButton->OnClicked.AddDynamic(this, &ThisClass::TransactionHistoryButtonClicked);
	}

	ensure(WBP_ItemList);
	ensure(WBP_ItemFilter);
	if (IsValid(WBP_ItemFilter))
	{
		WBP_ItemFilter->OnSearch.AddDynamic(this, &ThisClass::Search);
	}
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

void UMAAuctionWidget::ItemSearchButtonClicked()
{
	if (MenuButtonClicked(ItemSearchButton))
	{
		WBP_ItemList->UpdateSearchItems(WBP_ItemFilter->GetCurrentOption());
	}
}

void UMAAuctionWidget::ItemBidOnButtonClicked()
{
	if (MenuButtonClicked(ItemBidOnButton))
	{
		WBP_ItemList->UpdateMyItems(EMyItemReqType::Buy);
	}
}

void UMAAuctionWidget::ItemRegisteredButtonClicked()
{
	if (MenuButtonClicked(ItemRegisteredButton))
	{
		WBP_ItemList->UpdateMyItems(EMyItemReqType::Sell);
	}
}

void UMAAuctionWidget::RegisterItemButtonClicked()
{
	if (MenuButtonClicked(RegisterItemButton))
	{

	}
}

void UMAAuctionWidget::TransactionHistoryButtonClicked()
{
	if (MenuButtonClicked(TransactionHistoryButton))
	{

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
	WBP_ItemList->UpdateSearchItems(InOption);
}