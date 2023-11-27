// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAWindowWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAWindowWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

private:

	UFUNCTION()
	void CloseButtonClicked();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, Category = "CloseButton", meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> CloseButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CloseButton", meta = (BindWidget, AllowPrivateAccess = "true"))
	FText OverrideTitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CloseButton", meta = (BindWidget, AllowPrivateAccess = "true"))
	ESlateVisibility OverrideCloseButtonVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CloseButton", meta = (BindWidget, AllowPrivateAccess = "true"))
	uint8 bUseGameInputModeThenClose : 1;
};
