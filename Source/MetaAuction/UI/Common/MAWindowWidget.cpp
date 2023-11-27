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

		TArray<UUserWidget*> AllWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), true);

		// Action Widget의 Visibility를 비교하여 Visible이 아니면 꺼진 상태로 간주
		const int32 AllWidgetsNum = AllWidgets.Num() - static_cast<int32>(MAAuctionWidget->GetVisibility() != ESlateVisibility::Visible);

		// 모든 위젯의 개수가 자기자신 포함 2개(자신, HUD)이면 어떠한 위젯도 열려있지 않으므로 InputMode GameOnly로 변경
		if (bUseGameInputModeThenClose && (!IsValid(MAAuctionWidget) || AllWidgetsNum == 2))
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