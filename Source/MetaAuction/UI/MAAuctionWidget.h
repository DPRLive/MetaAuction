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

public:

	UFUNCTION(BlueprintCallable)
	void ToggleWidget();

	FORCEINLINE EItemCanDeal GetCachedItemCanDeal() { return CachedItemCanDeal; };

private:

	UFUNCTION()
	void ItemSearchButtonClicked();

	UFUNCTION()
	void ItemBidOnButtonClicked();

	UFUNCTION()
	void ItemRegisteredButtonClicked();

	UFUNCTION()
	void RegisterItemButtonClicked();

	UFUNCTION()
	void TransactionHistoryButtonClicked();

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
	TObjectPtr<class UButton> ItemBidOnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemRegisteredButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> RegisterItemButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> TransactionHistoryButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemFilterWidget> WBP_ItemFilter;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemTileWidget> WBP_ItemView;

	TArray<TObjectPtr<class UButton>> AllMenuButtons;

	TObjectPtr<class UButton> CurrentButton;
	TObjectPtr<class UButton> PreviousButton;

	EItemCanDeal CachedItemCanDeal;
};