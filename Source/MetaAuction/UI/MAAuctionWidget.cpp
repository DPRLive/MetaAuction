// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAAuctionWidget.h"
#include "UI/Item/MAItemTileWidget.h"
#include "UI/Item/MAItemFilterWidget.h"
#include "UI/Chat/MAChatInfoWidget.h"
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
	ensure(ItemSellButton);
	ensure(ItemBuyButton);
	ensure(ItemTryBidButton);
	ensure(ChatInfoButton);
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

	if (IsValid(ItemSellButton))
	{
		ItemSellButton->OnClicked.AddDynamic(this, &ThisClass::ItemSellButtonClicked);
	}

	if (IsValid(ItemBuyButton))
	{
		ItemBuyButton->OnClicked.AddDynamic(this, &ThisClass::ItemBuyButtonClicked);
	}

	if (IsValid(ItemTryBidButton))
	{
		ItemTryBidButton->OnClicked.AddDynamic(this, &ThisClass::ItemTryBidButtonClicked);
	}

	if (IsValid(ChatInfoButton))
	{
		ChatInfoButton->OnClicked.AddDynamic(this, &ThisClass::ChatInfoButtonClicked);
	}

	if (IsValid(WBP_ItemFilter))
	{
		WBP_ItemFilter->OnSearch.AddDynamic(this, &ThisClass::Search);
	}

	ItemSearchButtonClicked();
}

FReply UMAAuctionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		if (CurrentButton == ItemSearchButton)
		{
			ItemSearchButtonClicked();
		}
		else if (CurrentButton == ItemSellButton)
		{
			ItemSellButtonClicked();
		}
		else if (CurrentButton == ItemBuyButton)
		{
			ItemBuyButtonClicked();
		}
		else if (CurrentButton == ItemTryBidButton)
		{
			ItemTryBidButtonClicked();
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
		SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		if (bFlushInput)
		{
			PlayerController->FlushPressedKeys();
		}

		PlayerController->SetShowMouseCursor(false);

		NotifyHiddenWidget();
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);

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
		const FItemSearchOption& ItemSearchOption = WBP_ItemFilter->GetCurrentOption();
		CachedItemCanDeal = ItemSearchOption.CanDeal;
		WBP_ItemView->UpdateSearchItems(ItemSearchOption);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMAAuctionWidget::ItemSellButtonClicked()
{
	if (MenuButtonClicked(ItemSellButton))
	{
		CachedItemCanDeal = EItemCanDeal::Possible;
		WBP_ItemView->UpdateMyItems(EMyItemReqType::Sell);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::ItemBuyButtonClicked()
{
	if (MenuButtonClicked(ItemBuyButton))
	{
		CachedItemCanDeal = EItemCanDeal::Possible;
		WBP_ItemView->UpdateMyItems(EMyItemReqType::Buy);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::ItemTryBidButtonClicked()
{
	if (MenuButtonClicked(ItemTryBidButton))
	{
		CachedItemCanDeal = EItemCanDeal::Possible;
		WBP_ItemView->UpdateMyItems(EMyItemReqType::TryBid);
		WBP_ItemFilter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAuctionWidget::ChatInfoButtonClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (UMAChatInfoWidget* ChatInfoWidget = CreateWidget<UMAChatInfoWidget>(PC, ChatInfoWidgetClass))
		{
			ChatInfoWidget->AddToViewport();
			ChatInfoWidget->Update();
		}
	}
}

bool UMAAuctionWidget::MenuButtonClicked(UButton* ClickedButton)
{
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
