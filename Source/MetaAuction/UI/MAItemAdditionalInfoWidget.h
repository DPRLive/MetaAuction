// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAItemAdditionalInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAItemAdditionalInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemAdditionalInfoWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	void Update(const FItemData& InItemData);

private:

	void AddItemReplyToChatLog(const FItemReply& InItemReply);

	UFUNCTION()
	void InputButtonClicked();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> InformationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> CommentText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableTextBox> InputText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> InputButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAChatLogListWidget> WBP_CommentList;

	UPROPERTY()
	FItemData CachedItemData;

	FDelegateHandle ItemReplyHandle;
	int32 CommentCount;
};
