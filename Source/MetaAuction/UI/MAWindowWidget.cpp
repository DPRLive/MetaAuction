// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAWindowWidget.h"

#include <Components/TextBlock.h>
#include <Components/Button.h>

UMAWindowWidget::UMAWindowWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OverrideCloseButtonVisibility = ESlateVisibility::Visible;
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
	UWidget* ParentWidget = Cast<UWidget>(GetParent()->GetOuter()->GetOuter());
	if (IsValid(ParentWidget))
	{
		ParentWidget->RemoveFromParent();
	}
}