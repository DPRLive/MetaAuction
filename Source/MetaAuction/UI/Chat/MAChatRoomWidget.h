// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAChatRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAChatRoomWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatRoomWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	void Update(const FChatRoomData& InChatRoomData);
	void SetFocusInputText();

private:

	UFUNCTION()
	void InputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void InputTextChanged(const FText& Text);

	UFUNCTION()
	void SendInputText();

	UFUNCTION()
	void ReceivedChatLog(const FChatData& InData);

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> PriceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UScrollBox> InputScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableTextBox> InputText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> InputButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAChatLogListWidget> WBP_ChatLogList;

	UPROPERTY(Transient)
	FChatRoomData CachedChatRoomData;

	FDelegateHandle OnChatDelegateHandle;

	uint8 bIsInit : 1;
};
