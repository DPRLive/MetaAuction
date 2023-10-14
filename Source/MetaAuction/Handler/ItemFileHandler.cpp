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
	// TODO: 사용되지 않을 로컬 모델링 파일 제거, 사진 파일 요청 필요
}

/**
 * 해당 item ID의 모델링 파일을 지웁니다.
 * @param InItemId : 지울 item의 id
 */
void FItemFileHandler::RemoveGlbFile(uint32 InItemId) const
{
	// 모델링 파일 경로에 접근한다.
	FString glbPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d.glb"), InItemId);
	
	IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();
	fileManager.DeleteFile(*glbPath);
}

/**
 * 해당 item ID의 모델링 파일(glb)를 요청합니다.
 * @param InFunc : 요청이 완료되면 실행할 함수
 * @param InItemId : 물품의 ItemId 
 */
void FItemFileHandler::RequestGlb(FRequestGlbCallback InFunc, uint32 InItemId) const
{
	// 모델링 파일 경로에 접근한다.
	FString glbPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d.glb"), InItemId);
	
	// FileExists가 case sensitive 하지 않네
	if (FPaths::FileExists(glbPath)) // 우선 로컬에 존재하는지 확인
	{
		if(InFunc)
		{
			InFunc(glbPath);
		}
		return;
	}

	// 없네. 요청
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		// HTTP 통신으로 관련 파일을 요청한다.
		LOG_N(TEXT("Request Item ID (%d) glb Files ..."), InItemId);
		
		if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
		{
			TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetModelHandler(); // 만약을 대비해 Weak 캡처 추가
			httpHandler->Request(DA_NETWORK(GlbFileDownAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,
				[thisPtr, InFunc, glbPath](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
			                     {
				                     if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
				                     {
					                     // 파일을 저장한다.
					                     thisPtr.Pin()->_OnGlbRequestCompleted(InResponse);

					                     if (InFunc)
					                     {
						                     InFunc(glbPath);
					                     }
				                     }
				                     else
				                     {
					                     LOG_ERROR(TEXT("ItemID : %s, 파일 요청 실패"), *glbPath);
				                     }
			                     });
		}
	}
	
}

/**
 * glb 파일 요청이 완료되면 호출될 함수, /Saved/Models 아래에 파일을 저장한다.
 * @param InResponse : http 응답
 */
void FItemFileHandler::_OnGlbRequestCompleted(FHttpResponsePtr InResponse) const
{
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("Models/");

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
		LOG_N(TEXT("%s, file download is complete!"), *fileName);
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
