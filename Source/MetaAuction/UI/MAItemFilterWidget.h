// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAItemFilterWidget.generated.h"

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

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UEditableText> SearchText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> SearchButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UComboBox> ItemDealTypeComboBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UComboBox> ItemCanDealComboBox;
};
