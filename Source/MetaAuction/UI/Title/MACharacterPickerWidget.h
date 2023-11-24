// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MACharacterPickerWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMACharacterPickerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMACharacterPickerWidget(const FObjectInitializer& ObjectInitializer);
	
protected:

	virtual void NativeConstruct() override;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> CharacterImage;
};
