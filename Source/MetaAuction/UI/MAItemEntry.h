// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Actor/ItemActor.h"
#include "MAItemEntry.generated.h"

/** UMAItemEntry
 * UMAItemListWidget의 리스트뷰에서 엔트리 오브젝트로 사용됩니다.
 */
UCLASS(BlueprintType, Blueprintable)
class METAAUCTION_API UMAItemEntry : public UObject
{
	GENERATED_BODY()
	
public:

	UMAItemEntry();
	
	UPROPERTY()
	FItemData ItemData;
};
