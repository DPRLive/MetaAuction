// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MAWidgetUtils.generated.h"
/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAWidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = ItemManager)
	static class UItemManager* GetItemManager(UWorld* World);

	static const TSharedPtr<class FItemFileHandler> GetItemFileHandler(UWorld* World);
};
