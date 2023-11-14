// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAConfirmCancelPopupWidget.h"

#include <Components/TextBlock.h>
#include <Components/Button.h>

UMAConfirmCancelPopupWidget::UMAConfirmCancelPopupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAConfirmCancelPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(ConfirmButton);
	ensure(CancelButton);

	if (IsValid(ConfirmButton))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::CancelButtonClicked);
	}
}

void UMAConfirmCancelPopupWidget::SetText(const FString& InString)
{
	TitleText->SetText(FText::FromString(InString));
}

void UMAConfirmCancelPopupWidget::ConfirmButtonClicked()
{
	RemoveFromParent();

	OnDetermined.Broadcast(EMAConfirmCancelPopupType::Confirm);
}

void UMAConfirmCancelPopupWidget::CancelButtonClicked()
{
	RemoveFromParent();
	OnDetermined.Broadcast(EMAConfirmCancelPopupType::Cancel);
}
