// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/MAWindowWidget.h"
#include "UI/MAAuctionWidget.h"
#include "Player/MAPlayerController.h"

#include <Components/TextBlock.h>
#include <Components/Button.h>

UMAWindowWidget::UMAWindowWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OverrideCloseButtonVisibility = ESlateVisibility::Visible;
	bUseGameInputModeThenClose = true;
}

void UMAWindowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(TitleText))
	{
		TitleText->SetText(OverrideTitleText);
	}

	if (IsValid(CloseButton))
	{
		CloseButton->SetVisibility(OverrideCloseButtonVisibility);
	}
}

void UMAWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(CloseButton);

	if (IsValid(CloseButton))
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseButtonClicked);
	}
}

void UMAWindowWidget::CloseButtonClicked()
{
	AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer());
	if (IsValid(MAPC))
	{
		UMAAuctionWidget* MAAuctionWidget = MAPC->GetAuctionWidget();
		if (IsValid(MAAuctionWidget))
		{
			MAAuctionWidget->SetFocus();
		}

		// InputMode GameOnly로 변경
		if (bUseGameInputModeThenClose && (!IsValid(MAAuctionWidget) || MAAuctionWidget->GetVisibility() != ESlateVisibility::Visible))
		{
			FInputModeGameOnly InputMode;
			MAPC->SetInputMode(InputMode);
			MAPC->FlushPressedKeys();
			MAPC->SetShowMouseCursor(false);
		}
	}

	UWidget* ParentWidget = Cast<UWidget>(GetParent()->GetOuter()->GetOuter());
	if (IsValid(ParentWidget))
	{
		ParentWidget->RemoveFromParent();
	}
}