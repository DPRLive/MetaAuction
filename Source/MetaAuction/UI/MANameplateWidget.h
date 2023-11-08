// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MANameplateWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMANameplateWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UMANameplateWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void SetName(const FText& InName);

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> NameText;
};