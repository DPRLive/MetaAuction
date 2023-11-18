// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UMAHUDWidget(const FObjectInitializer& ObjectInitializer);

	// 상호작용 UI를 토글합니다.
	void ToggleInteract(const bool IsOn, const FText& InInteractInfo = FText::FromString(TEXT("상호작용"))) const;
protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

	// 상호작용 UI 입니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UBorder> InteractUI;
	
	// 상호작용 정보를 표시하는 text 입니다. ex) F키를 눌러 '트랜스폼 수정' 에서 '' 부분
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> InteractInfo;
};