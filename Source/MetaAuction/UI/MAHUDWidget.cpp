// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAHUDWidget.h"

#include <Components/TextBlock.h>
#include <Components/Border.h>

UMAHUDWidget::UMAHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

/**
 *	상호작용 UI를 토글합니다.
 *	@param IsOn : 토글 여부
 *	@param InInteractInfo : 상호작용 정보. ex) F키를 눌러 '트랜스폼 수정' 에서 '' 부분
 */
void UMAHUDWidget::ToggleInteract(const bool IsOn, const FText& InInteractInfo) const
{
	InteractInfo->SetText(InInteractInfo);

	if(IsOn)
	{
		InteractUI->SetVisibility(ESlateVisibility::HitTestInvisible);
		return;
	}
	
	InteractUI->SetVisibility(ESlateVisibility::Hidden);
}

void UMAHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(CanvasPanel);
	ensure(InteractUI);
	ensure(InteractInfo);

	InteractUI->SetVisibility(ESlateVisibility::Hidden);
}