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

	UFUNCTION(BlueprintCallable)
	void ToggleWidget();
};
