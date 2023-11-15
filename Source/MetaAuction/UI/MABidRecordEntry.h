// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Handler/ItemDataHandler.h"
#include "MABidRecordEntry.generated.h"

/**UMABidRecordEntry
 * 
 */
UCLASS()
class METAAUCTION_API UMABidRecordEntry : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FBidRecord Data;
};
