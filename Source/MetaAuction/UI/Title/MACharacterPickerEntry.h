// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MACharacterPickerEntry.generated.h"

/**UMACharacterPickerEntry
 * UMACharacterPickerEntryWidget의 ItemEntry 입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterPickerEntry : public UObject
{
	GENERATED_BODY()
	
public:

	int32 Index;

	UPROPERTY()
	FCharacterMeshInfo Data;
};
