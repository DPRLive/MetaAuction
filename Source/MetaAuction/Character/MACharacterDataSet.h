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

	// 카메라 데이터를 여러개 설정하여 시점을 변경할 수 있도록 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	TArray<TObjectPtr<class UMACameraModeData>> CameraModeDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	TObjectPtr<class UMAInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	TObjectPtr<class UInputMappingContext> InputMapping;
};
