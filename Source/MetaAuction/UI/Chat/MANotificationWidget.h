// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "MANotificationWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

/**
 *	HUD에 붙어 알림이 오면 알림을 보여주는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMANotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	// 새로운 알림이 오면 처리합니다.
	void _OnReceiveNotification(const FString& InTitle, const FString& InContent, const FDateTime& InTime);

	// 애니메이션이 끝나면 처리합니다.
	UFUNCTION()
	void _AnimFinished();

	FWidgetAnimationDynamicEvent OnAnimFinished;

	// 위젯 애니메이션입니다.
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BubbleAnim;

	// 알림 제목입니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TitleText;

	// 알림 내용입니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> ContentText;

	// 알림이 온 시간입니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TimeText;
};
