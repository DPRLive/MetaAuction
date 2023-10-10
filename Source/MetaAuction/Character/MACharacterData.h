// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "Engine/DataAsset.h"
#include "MACharacterData.generated.h"

/**
 * 캐릭터에 적용할 수 있는 데이터 애셋의 기본적인 상위 클래스입니다.
 */
UCLASS(Abstract)
class METAAUCTION_API UMACharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	/**
	* 해당 데이터 애셋의 값을 캐릭터에 적용합니다.
	*/
	virtual void GiveToData(class AMACharacter* InCharacter) PURE_VIRTUAL(UMACharacterData::GiveToData, return;);
	
};
