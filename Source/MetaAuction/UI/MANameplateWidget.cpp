// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MANameplateWidget.h"
#include "Components/TextBlock.h"

UMANameplateWidget::UMANameplateWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMANameplateWidget::NativeConstruct()
{
	ensure(NameText);
}

void UMANameplateWidget::SetName(const FText& InName)
{
	if (IsValid(NameText))
	{
		NameText->SetText(InName);
	}
}
