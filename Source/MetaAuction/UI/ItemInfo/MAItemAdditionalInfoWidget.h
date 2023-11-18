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

public:

	void Update(const FItemData& InChatData);

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> InformationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMABidRecordListWidget> WBP_BidRecordList;

	UPROPERTY()
	FItemData CachedItemData;

	FDelegateHandle ItemReplyHandle;
	int32 CommentCount;
};
