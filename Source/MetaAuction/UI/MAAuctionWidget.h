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

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UHorizontalBox> MenuBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemSearchButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemBidOnButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ItemRegisteredButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> RegisterItemButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> TransactionHistoryButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemFilterWidget> WBP_ItemFilter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAItemListWidget> WBP_ItemList;

	TArray<TObjectPtr<class UButton>> AllMenuButtons;

	TObjectPtr<class UButton> CurrentButton;
	TObjectPtr<class UButton> PreviousButton;
};