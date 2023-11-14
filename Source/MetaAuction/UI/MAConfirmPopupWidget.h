// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAConfirmPopupWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FMAConfirmPopupDelegate);

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAConfirmPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAConfirmPopupWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable)
	void SetText(const FString& InString);

private:

	UFUNCTION()
	void ConfirmButtonClicked();

public:

	FMAConfirmPopupDelegate OnDetermined;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ConfirmButton;
};
