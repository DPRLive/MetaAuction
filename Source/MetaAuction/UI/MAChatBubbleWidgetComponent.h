// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "UI/MAChatLogEntry.h"
#include "MAChatBubbleWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAChatBubbleWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UMAChatBubbleWidgetComponent(const FObjectInitializer& ObjectInitializer);

public:

	void ShowChatBubble(const FString& ChatLog);
};
