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

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

};