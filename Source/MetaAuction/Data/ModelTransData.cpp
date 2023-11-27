// Fill out your copyright notice in the Description page of Project Settings.


#include "ModelTransData.h"

#include <HAL/FileManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModelTransData)

UModelTransData::UModelTransData()
{
	SaveTermCount = 10;
	NowChangeCount = 0;
}

/**
 * 서버가 종료 전 저장을 합니다.
 */
void UModelTransData::BeginDestroy()
{
	_SaveDataToFile();
	
	Super::BeginDestroy();
}

/**
 * 파일에서 TMap에 정보를 로드한다.
 */
void UModelTransData::LoadData()
{
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/ModelTrans.bin");
	
	if(FPaths::FileExists(basePath)) // 파일이 존재 하면 읽는다.
	{
		if(TUniquePtr<FArchive> fileReaderAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*basePath)))
		{
			ItemModelTrans.Empty();
			*fileReaderAr << ItemModelTrans;
			fileReaderAr->Close();
		}
	}
}

/**
 * 트랜스폼 정보를 등록 시도합니다. (이미 존재할 시 수정)
 */
void UModelTransData::TryEmplaceTrans(const uint8 InItemLoc, const uint32 InItemId, const FTransform& InTransform)
{
	if(TPair<uint32, FTransform>* modelTrans = ItemModelTrans.Find(InItemLoc))
		*modelTrans = {InItemId, InTransform};
	else
		ItemModelTrans.Emplace(InItemLoc, {InItemId, InTransform});
	
	_OnChanged();
}

/**
 * 트랜스폼 정보를 지웁니다.
 */
void UModelTransData::RemoveTrans(const uint8 InItemLoc)
{
	ItemModelTrans.Remove(InItemLoc);
	_OnChanged();
}

/**
 * 변경이 일어나면 Count를 확인해보고 파일을 저장하던지 합니다.
 */
void UModelTransData::_OnChanged()
{
	NowChangeCount++;

	if(NowChangeCount >= SaveTermCount)
		_SaveDataToFile();
}

/**
 * TMap의 내용을 저장한다.
 */
void UModelTransData::_SaveDataToFile()
{
	UE_SCOPED_TIMER(TEXT("Save Transform to bin..."), LogTemp, Warning);
	
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/ModelTrans.bin");

	if(TUniquePtr<FArchive> fileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*basePath)))
	{
		*fileWriterAr << ItemModelTrans;
		fileWriterAr->Close();
	}
}