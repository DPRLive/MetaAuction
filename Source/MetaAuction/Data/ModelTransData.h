// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/NoExportTypes.h>
#include "ModelTransData.generated.h"

/**
 *  item actor에 배치된 Model의 Transform 정보들을
 *  Serialization을 통해 관리해줍니다.
 */
UCLASS()
class METAAUCTION_API UModelTransData : public UObject
{
	GENERATED_BODY()

public:
	UModelTransData();

	virtual void BeginDestroy();
	
	// 파일에서 TMap에 정보를 로드한다.
	void LoadData();

	// 트랜스폼 정보를 등록 시도합니다. (이미 존재할 시 수정)
	void TryEmplaceTrans(const uint8 InItemLoc, const uint32 InItemId, const FTransform& InTransform);

	// 트랜스폼 정보를 지웁니다.
	void RemoveTrans(const uint8 InItemLoc);
	
	// ItemModelTrans Getter
	FORCEINLINE const TMap<uint8, TPair<uint32, FTransform>>& GetItemModelTrans() const { return ItemModelTrans; }
	
private:
	// 변경이 일어나면 Count를 확인해보고 파일을 저장하던지 합니다.
	void _OnChanged();
	
	// TMap의 내용을 파일에 저장한다.
	void _SaveDataToFile();

	// 몇번에 한번씩 로컬 파일에 정보를 저장할 것인지 설정합니다.
	UPROPERTY( EditDefaultsOnly, Category = "Setting", meta = (AllowPrivateAccess = true) )
	uint8 SaveTermCount;

	UPROPERTY( Transient )
	uint8 NowChangeCount;
	
	// {ItemActor의 위치 , {Item Actor에 배치된 item id & 그린 모델의 Transform에 대한 설정값}} 입니다.
	TMap<uint8, TPair<uint32, FTransform>> ItemModelTrans;
};