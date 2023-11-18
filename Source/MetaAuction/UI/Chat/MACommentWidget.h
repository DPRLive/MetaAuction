// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MACommentWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMACommentWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMACommentWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	void Update(const FItemData& InChatData);

private:

	void AddChatDataToChatLog(const FChatData& InChatData);

	UFUNCTION()
	void InputButtonClicked();

private:

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
