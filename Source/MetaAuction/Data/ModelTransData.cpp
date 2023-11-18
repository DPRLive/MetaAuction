// Fill out your copyright notice in the Description page of Project Settings.


#include "ModelTransData.h"

#include <HAL/FileManager.h>

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
	
	_SaveChanged();
}

/**
 * 트랜스폼 정보를 지웁니다.
 */
void UModelTransData::RemoveTrans(const uint8 InItemLoc)
{
	ItemModelTrans.Remove(InItemLoc);
	_SaveChanged();
}

/**
 * TMap의 내용을 저장한다.
 * TODO : 수정이나 삭제 잘 되는지 확인 필요
 */
void UModelTransData::_SaveChanged()
{
	UE_SCOPED_TIMER(TEXT("Save Transform to bin..."), LogTemp, Warning);
	
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/ModelTrans.bin");

	if(TUniquePtr<FArchive> fileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*basePath)))
	{
		*fileWriterAr << ItemModelTrans;
		fileWriterAr->Close();
	}
}