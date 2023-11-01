// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Manager/ItemManager.h"
#include "MAItemFilterWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FItemSearchOption&, Option);


/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAItemFilterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemFilterWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable)
	void Search();

	FItemSearchOption GetCurrentOption();

private:

	UFUNCTION()
	void SearchButtonClicked();

	UFUNCTION()
	void SearchTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:

	FSearchSignature OnSearch;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableTextBox> SearchText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> SearchButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UComboBoxString> ItemWorldTypeComboBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UComboBoxString> ItemDealTypeComboBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UComboBoxString> ItemCanDealComboBox;
};
