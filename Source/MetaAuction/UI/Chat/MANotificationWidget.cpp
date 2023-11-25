// Fill out your copyright notice in the Description page of Project Settings.


#include "MANotificationWidget.h"

#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MANotificationWidget)

void UMANotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(ContentText);
	ensure(TimeText);

	SetVisibility(ESlateVisibility::Hidden);
	
	// Notification Manager로 부터 알림을 받습니다.
	if(FNotificationManager* notiManager = MAGetNotificationManager(GetGameInstance()))
	{
		notiManager->OnNotificationDelegate.AddUObject(this, &UMANotificationWidget::_OnReceiveNotification);
	}
}

/**
 * 새로운 알림이 오면 처리합니다.
 */
void UMANotificationWidget::_OnReceiveNotification(const FString& InTitle, const FString& InContent, const FDateTime& InTime)
{
	if(IsValid(TitleText) && IsValid(ContentText) && IsValid(TimeText))
	{
		// 설정하고
		TitleText->SetText(FText::FromString(InTitle));
		ContentText->SetText(FText::FromString(InContent));
		TimeText->SetText(FText::FromString(InTime.ToString()));
		SetVisibility(ESlateVisibility::HitTestInvisible);

		// 애니메이션 준비하고
		OnAnimFinished.Clear();
		OnAnimFinished.BindDynamic(this, &UMANotificationWidget::_AnimFinished);
		BindToAnimationFinished(BubbleAnim, OnAnimFinished);
		
		PlayAnimation(BubbleAnim);
	}
}

/**
 * 애니메이션이 끝나면 처리합니다.
 */
void UMANotificationWidget::_AnimFinished()
{
	SetVisibility(ESlateVisibility::Hidden);
}