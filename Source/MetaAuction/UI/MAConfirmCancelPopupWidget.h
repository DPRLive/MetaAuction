// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAConfirmCancelPopupWidget.generated.h"

UENUM(BlueprintType)
enum class EMAConfirmCancelPopupType : uint8
{
	Confirm,
	Cancel
};

DECLARE_MULTICAST_DELEGATE_OneParam(FMAConfirmCancelPopupDelegate, EMAConfirmCancelPopupType);

/**UMAConfirmCancelPopupWidget
 * 사용자가 한 작업이 확실한지 한번 더 묻기 위한 UI입니다.
 */
UCLASS()
class METAAUCTION_API UMAConfirmCancelPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAConfirmCancelPopupWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	
public:

	UFUNCTION(BlueprintCallable)
	void SetText(const FString& InString);

private:

	UFUNCTION()
	void ConfirmButtonClicked();

	UFUNCTION()
	void CancelButtonClicked();

public:

	FMAConfirmCancelPopupDelegate OnDetermined;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ConfirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> CancelButton;
};
