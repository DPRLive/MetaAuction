// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatBubbleWidget.h"

#include <Components/TextBlock.h>
#include <Animation/WidgetAnimation.h>

UMAChatBubbleWidget::UMAChatBubbleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UMAChatBubbleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LogText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LogText")));
	ensure(LogText);

	OnBubbleAnimFinished.BindDynamic(this, &ThisClass::BubbleAnimFinished);
	BindToAnimationFinished(BubbleAnim, OnBubbleAnimFinished);

	SetVisibility(ESlateVisibility::Hidden);
}

void UMAChatBubbleWidget::ShowChatBubble(const FString& ChatLog)
{
	if (IsValid(BubbleAnim))
	{
		LogText->SetText(FText::FromString(ChatLog));
		SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(BubbleAnim);
	}
}

void UMAChatBubbleWidget::BubbleAnimFinished()
{
	SetVisibility(ESlateVisibility::Hidden);
}
