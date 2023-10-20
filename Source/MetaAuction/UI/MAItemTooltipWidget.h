// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actor/ItemActor.h"
#include "MAItemTooltipWidget.generated.h"

struct FItemData;
/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemTooltipWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void UpdateById(uint32 InItemID);

private:

	UFUNCTION()
	void ItemImagePrevButtonClicked();

	UFUNCTION()
	void ItemImageNextButtonClicked();

	void UpdateAll(const FItemData& InItemData);
	void UpdateText(const FItemData& InItemData);
	void UpdateImage();
	void LoadAllImage(const FItemData& InItemData);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void AddImage(const FString& InFilePath);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> InformationText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> BuyerNameText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> StartPriceText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> CurrentPriceText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> EndTimeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImagePrevButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImageNextButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UTexture2D>> CachedTextures;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	int32 CurrentImageIndex;
};
