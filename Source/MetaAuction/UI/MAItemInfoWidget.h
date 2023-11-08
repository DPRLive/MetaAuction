// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actor/ItemActor.h"
#include "MAItemInfoWidget.generated.h"

struct FItemData;

/**UMAItemInfoWidget
 */
UCLASS()
class METAAUCTION_API UMAItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemInfoWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void Update(const FItemData& InItemData);

private:

	void UpdateText(const FItemData& InItemData);
	void UpdateImage(const FItemData& InItemData);

	UFUNCTION()
	void ItemImagePrevButtonClicked();

	UFUNCTION()
	void ItemImageNextButtonClicked();

	UFUNCTION()
	void DetailsButtonClicked();

	FDelegateHandle ItemImageListViewSelectionChangedHandle;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> BuyerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> StartPriceText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> CurrentPriceText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> EndTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImagePrevButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImageNextButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemImageListWidget> WBP_ItemImageList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> DetailsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMAItemAdditionalInfoWidget> ItemAdditionalInfoWidgetClass;

	UPROPERTY()
	FItemData CachedItemData;
};
