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

	// �� ������ FItemData ������� ��� ������Ʈ �մϴ�.
	void UpdateAll(const FItemData& InItemData);

private:

	// ItemImagePrevButton�� Ŭ���Ǿ��� �� ȣ��Ǵ� �Լ��Դϴ�.
	UFUNCTION()
	void ItemImagePrevButtonClicked();

	// ItemImageNextButton�� Ŭ���Ǿ��� �� ȣ��Ǵ� �Լ��Դϴ�.
	UFUNCTION()
	void ItemImageNextButtonClicked();

	void UpdateText(const FItemData& InItemData);
	void UpdateImage();
	void LoadAllImage(const FItemData& InItemData);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void AddImage(const FString& InFilePath);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> InformationText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> BuyerNameText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> StartPriceText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> CurrentPriceText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> EndTimeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImagePrevButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemImageNextButton;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UTexture2D>> CachedTextures;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentImageIndex;
};
