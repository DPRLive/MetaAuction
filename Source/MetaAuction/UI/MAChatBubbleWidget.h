// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimationEvents.h"
#include "MAChatBubbleWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAChatBubbleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatBubbleWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void ShowChatBubble(const FString& ChatLog);

private:

	UFUNCTION()
	void BubbleAnimFinished();

private:

	FWidgetAnimationDynamicEvent OnBubbleAnimFinished;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> BubbleAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> LogText;
};
