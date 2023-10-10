// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "TestDataAsset.generated.h"

/**
 * 임시 테스트용
 */
UCLASS()
class METAAUCTION_API UTestDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere )
	FString MyString = TEXT("");
};
