// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MAWidgetHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAWidgetHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static void RequestImageByItemID(class UImage* ImageWidget, uint32 InItemID, uint8 InImageNumber = 1);
};
