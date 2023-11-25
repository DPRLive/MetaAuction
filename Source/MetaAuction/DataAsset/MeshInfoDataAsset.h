// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "MeshInfoDataAsset.generated.h"

USTRUCT( BlueprintType )
struct FCharacterMeshInfo
{
	GENERATED_BODY()

	FCharacterMeshInfo() : MeshImg(nullptr), MeshPath(FSoftObjectPath()) {}

	// 메시에 관한 Img 입니다. TSoftObjectPtr 사용시 IsNull()로 체크 해주세요.
	UPROPERTY( EditDefaultsOnly )
	TSoftObjectPtr<UTexture2D> MeshImg;

	// 메시의 path 입니다.
	UPROPERTY( EditDefaultsOnly )
	FSoftObjectPath MeshPath;
};

/**
 * 캐릭터가 사용할 수 있는 캐릭터 메시들에 대한 정보입니다.
 */
UCLASS()
class METAAUCTION_API UMeshInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 캐릭터별 {메시의 사진, 메시의 경로}를 정의합니다.
	// TSoftObjectPtr 사용시 IsNull()로 체크 해주세요.
	UPROPERTY( EditDefaultsOnly )
	TArray<FCharacterMeshInfo> CharacterMeshes;
};
