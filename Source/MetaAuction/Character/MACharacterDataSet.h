// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "MACharacterData.h"
#include "MACharacterDataSet.generated.h"

/**
 * 캐릭터를 구성하는 모든 데이터의 모음입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterDataSet : public UMACharacterData
{
	GENERATED_BODY()
	
public:

	virtual void GiveToData(class AMACharacter* InCharacter) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Control")
	TObjectPtr<class UMACharacterControlData> CharacterControlData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Mesh")
	TObjectPtr<class UMACharacterMeshData> CharacterMeshData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	TObjectPtr<class UMACameraModeData> CameraModeData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	TObjectPtr<class UMAInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	TObjectPtr<class UInputMappingContext> InputMapping;
};
