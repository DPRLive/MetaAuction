#include "ItemFileHandler.h"
#include "Core/MAGameInstance.h"

#include <Interfaces/IHttpResponse.h>
#include <HAL/PlatformFileManager.h>
#include <GenericPlatform/GenericPlatformFile.h>

/**
 * 생성자
 */
FItemFileHandler::FItemFileHandler()
{
	// TODO: 사용되지 않을 로컬 모델링 파일 제거
}

/**
 * 필요한 파일들을 요청하는 함수
 * @param InItemID : 요청할 물품의 ID
 * @param InFileCount : 요청할 파일의 갯수 ( .glb 파일과 사진 파일들 합친 갯수 )
 */
void FItemFileHandler::RequestItemFiles(uint32 InItemID, uint8 InFileCount) const
{
	// 모델링 파일 경로에 접근한다.
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d"), InItemID);

	if (FPaths::DirectoryExists(basePath)) // 우선 로컬에 존재하는지 확인
	{
		IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();

		TArray<FString> files;
		fileManager.FindFiles(files, *basePath, nullptr);

		if (files.Num() == InFileCount) // 저장되어 있는 파일 갯수가 원래 요구하는 파일 갯수랑 같은지 확인해본다.
		{
			LOG_N(TEXT("file Exist!")); // 맞네 그럼 처리.
			
			for (const FString& file : files)
			{
				CompletedItemFileReq.Broadcast(InItemID, file);
			}
			return;
		}
		// 아니면 파일 지우고 다시 요청할거임.
		fileManager.DeleteDirectoryRecursively(*basePath);
	}

	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		// HTTP 통신으로 관련 파일을 요청한다.
		LOG_N(TEXT("Request Files ..."));

		// 파일 갯수만큼 요청을 한다.
		for (uint8 idx = 1; idx <= InFileCount; idx++)
		{
			if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
			{
				TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetModelHandler(); // 만약을 대비해 Weak 캡처 추가
				httpHandler->Request(DA_NETWORK(FileDownAddURL) + FString::Printf(TEXT("/%d/%d"), InItemID, idx),
				                     EHttpRequestType::GET,
				                     [InItemID, thisPtr](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
				                     {
					                     if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
					                     {
						                     // 파일을 저장한다.
						                     thisPtr.Pin()->_OnFileRequestCompleted(InItemID, InResponse);
					                     }
					                     else
					                     {
						                     LOG_ERROR(TEXT("ItemID : %d, 파일 요청 실패"), InItemID);
					                     }
				                     });
			}
		}
	}
}

/**
 * 해당 item ID의 파일들을 지웁니다.
 * @param InItemID : 지울 item의 id
 */
void FItemFileHandler::RemoveItemFiles(uint32 InItemID)
{
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d"), InItemID);

	if (FPaths::DirectoryExists(basePath)) // 존재하는지 확인
	{
		IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();
		fileManager.DeleteDirectoryRecursively(*basePath); // 지우기
	}
}

/**
 * 파일 요청이 완료되면 호출될 함수, 파일을 저장한다.
 * @param InItemID : 요청했던 물품의 ID
 * @param InResponse : http 응답
 */
void FItemFileHandler::_OnFileRequestCompleted(uint32 InItemID, FHttpResponsePtr InResponse) const
{
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d/"), InItemID);

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Dir Tree 만들기
	platformFile.CreateDirectoryTree(*basePath);

	// 파일명 그대로 받아오기
	FString fileName = _ParseInHeader(InResponse->GetHeader(TEXT("Content-disposition")));
	FString fileSavedPath = basePath + fileName;

	if (IFileHandle* fileHandler = platformFile.OpenWrite(*fileSavedPath))
	{
		fileHandler->Write(InResponse->GetContent().GetData(), InResponse->GetContentLength());
		fileHandler->Flush();

		delete fileHandler;
		LOG_N(TEXT("Item : %d, file download is complete!"), InItemID);

		// 파일 다운 완료 알림
		CompletedItemFileReq.Broadcast(InItemID, fileSavedPath);
	}
	else
	{
		LOG_ERROR(TEXT("file Handler is nullptr!"));
	}
}

/**
 * 헤더에서 ""을 기준으로 컨텐츠를 뽑아내주는 함수
 * @param InContent : header에서 뽑아낸 xxx = " ~~ " 형태의 FString
 */
FString FItemFileHandler::_ParseInHeader(const FString& InContent) const
{
	int32 startIdx, lastIdx;
	if (InContent.FindChar('\"', startIdx) && InContent.FindLastChar('\"', lastIdx))
	{
		return InContent.Mid(startIdx + 1, lastIdx - startIdx - 1);
	}

	return TEXT("/");
}
