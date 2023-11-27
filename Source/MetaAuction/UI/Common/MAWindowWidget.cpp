// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/MAWindowWidget.h"
#include "UI/MAAuctionWidget.h"
#include "Player/MAPlayerController.h"

#include <Blueprint/WidgetBlueprintLibrary.h>
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

	TitleText->SetText(OverrideTitleText);

	CloseButton->SetVisibility(OverrideCloseButtonVisibility);
}

void UMAWindowWidget::NativeDestruct()
{
	if (AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer()))
	{
		TArray<UUserWidget*> AllWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), true);
		int32 AllWidgetsNum = AllWidgets.Num();

		UMAAuctionWidget* MAAuctionWidget = MAPC->GetAuctionWidget();
		if (IsValid(MAAuctionWidget))
		{
			MAAuctionWidget->SetFocus();

			// Action Widget의 Visibility를 비교하여 Visible이 아니면 꺼진 상태로 간주
			AllWidgetsNum -= static_cast<int32>(MAAuctionWidget->GetVisibility() != ESlateVisibility::Visible);
		}

		// 모든 위젯의 개수가 1개(HUD)이면 어떠한 위젯도 열려있지 않으므로 InputMode GameOnly로 변경
		if (bUseGameInputModeThenClose && (!IsValid(MAAuctionWidget) || AllWidgetsNum == 1))
		{
			FInputModeGameOnly InputMode;
			MAPC->SetInputMode(InputMode);
			MAPC->FlushPressedKeys();
			MAPC->SetShowMouseCursor(false);
		}
	}

	Super::NativeDestruct();
}

void UMAWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseButtonClicked);
}

void UMAWindowWidget::CloseButtonClicked()
{
	if (UWidget* ParentWidget = Cast<UWidget>(GetParent()->GetOuter()->GetOuter()))
	{
		ParentWidget->RemoveFromParent();
	}
}