// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MALoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMALoginWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMALoginWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	UFUNCTION()
	void LoginButtonClicked();

	UFUNCTION()
	void RegisterButtonClicked();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableTextBox> UserIdText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableTextBox> PasswordText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> LoginButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> RegisterButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMACharacterPickerWidget> CharacterPickerWidgetClass;

	FDelegateHandle OnLoginDelegateHandle;
};
