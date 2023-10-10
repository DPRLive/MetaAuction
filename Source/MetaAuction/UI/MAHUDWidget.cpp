// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAHUDWidget.h"

#include <Components/CanvasPanel.h>

UMAHUDWidget::UMAHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UMAHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("BaseCanvasPanel")));
	ensure(CanvasPanel);
}