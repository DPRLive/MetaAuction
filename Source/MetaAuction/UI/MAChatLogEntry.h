// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MAChatLogEntry.generated.h"

/** FMAChatLogEntryData
 * UMAChatLogEntry를 이루고 있는 기본 데이터 입니다.
 */
USTRUCT(BlueprintType)
struct FMAChatLogEntryData
{
	GENERATED_BODY()

public:

	FMAChatLogEntryData(const FText& InChatName = FText(), const FText& InChatLog = FText());

	UPROPERTY()
	FText ChatName;

	UPROPERTY()
	FText ChatLog;
};

/** UMAChatLogEntry
 * UMAChatLogListWidget의 리스트뷰에서 엔트리 오브젝트로 사용됩니다.
 */
UCLASS(BlueprintType, Blueprintable)
class METAAUCTION_API UMAChatLogEntry : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FMAChatLogEntryData Data;
};