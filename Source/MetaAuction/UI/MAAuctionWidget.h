// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAAuctionWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAAuctionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UMAAuctionWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

	UFUNCTION(BlueprintCallable)
	void ToggleWidget();

	FORCEINLINE EItemCanDeal GetCachedItemCanDeal() { return CachedItemCanDeal; };

private:

	UFUNCTION()
	void ItemSearchButtonClicked();

	UFUNCTION()
	void ItemSellButtonClicked();

	UFUNCTION()
	void ItemBuyButtonClicked();

	UFUNCTION()
	void ItemTryBidButtonClicked();

	UFUNCTION()
	void ChatInfoButtonClicked();

	bool MenuButtonClicked(class UButton* ClickedButton);

	UFUNCTION()
	void Search(const FItemSearchOption& InOption);

	void NotifyVisibleWidget();
	void NotifyHiddenWidget();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UHorizontalBox> MenuBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemSearchButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemSellButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemBuyButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemTryBidButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ChatInfoButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemFilterWidget> WBP_ItemFilter;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemTileWidget> WBP_ItemView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMAChatInfoWidget> ChatInfoWidgetClass;

	TArray<TObjectPtr<class UButton>> AllMenuButtons;

	TObjectPtr<class UButton> CurrentButton;
	TObjectPtr<class UButton> PreviousButton;

	EItemCanDeal CachedItemCanDeal;
};