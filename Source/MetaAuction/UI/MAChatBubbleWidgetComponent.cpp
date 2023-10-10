// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAChatBubbleWidgetComponent.h"
#include "UI/MAChatBubbleWidget.h"
#include "Player/MAPlayerController.h"
#include "Common/MALog.h"

#include <GameFramework/Pawn.h>
#include <Animation/WidgetAnimation.h>
#include <EngineUtils.h>

UMAChatBubbleWidgetComponent::UMAChatBubbleWidgetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(300.0f, 200.0f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UMAChatBubbleWidgetComponent::ShowChatBubble(const FString& ChatLog)
{
	UMAChatBubbleWidget* ChatBubbleWidget = Cast<UMAChatBubbleWidget>(GetWidget());
	if (IsValid(ChatBubbleWidget))
	{
		ChatBubbleWidget->ShowChatBubble(ChatLog);
	}
}