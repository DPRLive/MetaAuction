// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAChatLogWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAChatLogWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatLogWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable)
	void EnableInputText();

	UFUNCTION(BlueprintCallable)
	void DisableInputText();

	UFUNCTION(BlueprintCallable)
	void SendInputText();

private:

	UFUNCTION()
	void InputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void InputTextChanged(const FText& Text);

	UFUNCTION()
	void ReceivedChatLog(const FMAChatLogEntryData& InData);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAChatLogListWidget> WBP_ChatLogList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UScrollBox> InputScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableText> InputText;
	

	uint8 bIsFocus : 1;

	int32 CurrentLogTextLine;
	int32 MaxLogTextLine;
};
