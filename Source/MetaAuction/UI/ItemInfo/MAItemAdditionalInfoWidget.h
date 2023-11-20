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

	void Update(const FItemData& InChatData);

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemModelImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMABidRecordListWidget> WBP_BidRecordList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform ItemDisplayerTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AMAItemDisplayer> ItemDisplayerClass;

	UPROPERTY(Transient)
	FItemData CachedItemData;

	UPROPERTY(Transient)
	TWeakObjectPtr<class AMAItemDisplayer> SpawnedItemDisplayer;

	FDelegateHandle ItemReplyHandle;
	int32 CommentCount;
};
