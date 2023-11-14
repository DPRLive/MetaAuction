// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAConfirmPopupWidget.h"

#include <Components/TextBlock.h>
#include <Components/Button.h>

UMAConfirmPopupWidget::UMAConfirmPopupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAConfirmPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(ConfirmButton);

	if (IsValid(ConfirmButton))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);
	}
}

void UMAConfirmPopupWidget::SetText(const FString& InString)
{
	TitleText->SetText(FText::FromString(InString));
}

void UMAConfirmPopupWidget::ConfirmButtonClicked()
{
	RemoveFromParent();

	OnDetermined.Broadcast();
}