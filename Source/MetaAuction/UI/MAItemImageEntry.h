// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MAItemImageEntry.generated.h"

/** FMAItemImageEntryData
 * UMAItemImageEntry�� �̷�� �ִ� �⺻ ������ �Դϴ�.
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
 * UMAItemImageListWidget�� ����Ʈ�信�� ��Ʈ�� ������Ʈ�� ���˴ϴ�.
 */
UCLASS(BlueprintType, Blueprintable)
class METAAUCTION_API UMAItemImageEntry : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FMAItemImageEntryData Data;
};