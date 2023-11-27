// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/MAConfirmCancelPopupWidget.h"

#include <Components/TextBlock.h>
#include <Components/Button.h>

UMAConfirmCancelPopupWidget::UMAConfirmCancelPopupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAConfirmCancelPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);

	CancelButton->OnClicked.AddDynamic(this, &ThisClass::CancelButtonClicked);
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
