// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAWindowWidget.h"

#include <Components/CanvasPanel.h>
#include <Components/Button.h>

UMAWindowWidget::UMAWindowWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OverrideCloseButtonVisibility = ESlateVisibility::Visible;
}

void UMAWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(CloseButton);

	if (IsValid(CloseButton))
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseButtonClicked);
	}
}

void UMAWindowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ensure(CloseButton);

	if (IsValid(CloseButton))
	{
		CloseButton->SetVisibility(OverrideCloseButtonVisibility);
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