// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MAItemImageEntry.generated.h"

/** FMAItemImageEntryData
 * UMAItemImageEntry를 이루고 있는 기본 데이터 입니다.
 */
USTRUCT(BlueprintType)
struct FMAItemImageEntryData
{
	GENERATED_BODY()

public:

	FMAItemImageEntryData(UTexture2DDynamic* InImage = nullptr);

	UPROPERTY()
	TObjectPtr<UTexture2DDynamic> Image;
};

/** UMAItemImageEntry
 * UMAItemImageListWidget의 리스트뷰에서 엔트리 오브젝트로 사용됩니다.
 */
UCLASS(BlueprintType, Blueprintable)
class METAAUCTION_API UMAItemImageEntry : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FMAItemImageEntryData Data;
};