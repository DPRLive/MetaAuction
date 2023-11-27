#include "ItemFileHandler.h"
#include "Core/MAGameInstance.h"

#include <Interfaces/IHttpResponse.h>
#include <HAL/PlatformFileManager.h>
#include <GenericPlatform/GenericPlatformFile.h>
#include <IImageWrapperModule.h>
#include <Modules/ModuleManager.h>
#include <Engine/Texture2DDynamic.h>
#include <RenderingThread.h>
#include <TextureResource.h>
#include <Serialization/JsonReader.h>
#include <Serialization/JsonSerializer.h>
#include <HAL/FileManagerGeneric.h>

/**
 * 생성자
 */
FItemFileHandler::FItemFileHandler()
{
}

/**
 * 캐시 파일들 (Saved/Models/에 저장되는 모델 파일들)을 지웁니다.
 * TODO : 아마 프로그램 실행 시 메인 화면에서 로그인 후 레벨 넘어가기 전 unuseable로 한번 호출해주면 좋을듯해요
 * @param InRemoveCacheType : 파일을 지울때의 옵션, Unuseable일 경우 로그인 된 상태에서 해주세요.
 */
void FItemFileHandler::RemoveCacheFile(ERemoveCacheType InRemoveCacheType) const
{
	FString basePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("Models/");
	
	if(InRemoveCacheType == ERemoveCacheType::All) // 다 지워버리렴!
	{
		IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
		platformFile.DeleteDirectoryRecursively(*basePath);

		return;
	}

	// 판매중인 물품은 캐시 파일 지울 목록에서 제외
	// 옵션 설정
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	requestObj->SetBoolField(TEXT("isPossible"), true);
	
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		httpHelper->Request(DA_NETWORK(ItemSearchAddURL), EHttpRequestType::POST,[basePath](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 TArray<TSharedPtr<FJsonObject>> jsonArray;
									 UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

								 	// 지우지 않아야 할 물품 번호들을 뽑는다
									 TSet<uint32> sellItemIds;
									 for (const TSharedPtr<FJsonObject>& itemInfo : jsonArray)
									 {
										 uint32 id;
										 if(itemInfo->TryGetNumberField(TEXT("id"), id))
										 {
										 	sellItemIds.Emplace(id);
										 }
									 }

								 	// 확인해가며 파일을 지운다
								 	TArray<FString> files;
								 	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
								 	platformFile.FindFiles(files, *basePath, nullptr);

								 	for(const FString& file : files)
								 	{
								 		FString path, fileName, extension;
								 		FPaths::Split(file, path, fileName, extension);

								 		if(sellItemIds.Find(FCString::Atoi(*fileName)) == nullptr)
								 		{
								 			platformFile.DeleteFile(*file);
								 		}
								 	}
								 }
							 }, UtilJson::JsonToString(requestObj));
	}
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
 * 해당 item ID의 모델링 파일(glb)를 요청합니다. 로컬에 있을경우 그 파일을 사용하며, 없을 경우 웹으로 새로 요청합니다. 
 * @param InFunc : 요청이 완료되면 실행할 람다 함수, 형식이 같아야 하며 람다 내부에서 클래스 멤버 접근시 weak 캡처 해주세요!
 * @param InItemId : 물품의 ItemId 
 */
void FItemFileHandler::RequestGlb(const FCallbackRefOneParam<FString>& InFunc, uint32 InItemId) const
{
	// 모델링 파일 경로에 접근한다.
	FString glbPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + FString::Printf(TEXT("Models/%d.glb"), InItemId);
	
	// FileExists가 case sensitive 하지 않네
	if (FPaths::FileExists(glbPath)) // 우선 로컬에 존재하는지 확인
	{
		// 존재하면 최신화된 파일인지 한번 확인한다.
		const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance());
		const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance());
		
		if ((httpHelper != nullptr) && (gameInstance != nullptr))
		{
			LOG_N(TEXT("Request itemID[%d] Last Modified time ..."), InItemId);

			// 파일의 수정 시각을 읽는다
			FFileManagerGeneric fileManager;
			FDateTime localFileTime = fileManager.GetTimeStamp(*glbPath);
			
			TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetItemFileHandler(); // 만약을 대비해 Weak 캡처 추가
			httpHelper->Request(DA_NETWORK(ModifyTimeAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,
			                     [thisPtr, InFunc, localFileTime, glbPath, InItemId](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
			                     {
				                     if (InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
				                     {
					                     // 현재 로컬 파일이 더 최신이면
					                     FDateTime serverModifiedTime;
					                     if (FDateTime::ParseIso8601(*InResponse->GetContentAsString(),serverModifiedTime) &&
						                     ((serverModifiedTime + DA_NETWORK(WebServerUTCDiff)) < localFileTime) && InFunc)
					                     {
						                     // 가져가라
						                     InFunc(glbPath);
						                     return;
					                     }

					                     // 아니면 다시 요청하셈..
					                     if (thisPtr.IsValid())
					                     {
						                     thisPtr.Pin()->_RequestGlbToWeb(InFunc, InItemId);
						                     return;
					                     }
				                     }
				                     LOG_ERROR(TEXT("URL : %s, Last Modified Time 요청 실패"), *InRequest->GetURL());
			                     });
		}
		return;
	}
	// 로컬에 없으면 새로 요청
	_RequestGlbToWeb(InFunc, InItemId);
}

/**
 * 해당 item Id의 index번째 이미지를 요청합니다. 이미지 인덱스 범위는 1 <= N <= ImgCnt 입니다.
 * @param InFunc : 요청이 완료되면 실행할 람다 함수, 형식이 같아야 하며 람다 내부에서 클래스 멤버 접근시 weak 캡처 해주세요! 
 * @param InItemId : 요청할 Item의 id
 * @param InImgIdx : 요청할 Item의 몇번째 사진인지?
 */
void FItemFileHandler::RequestImg(const FCallbackOneParam<UTexture2DDynamic*>& InFunc, uint32 InItemId, uint8 InImgIdx) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 관련 파일을 요청한다.
		LOG_N(TEXT("Request Item ID (%d) Img File (%d) ..."), InItemId, InImgIdx);
		
		if (const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
		{
			TWeakPtr<const FItemFileHandler> thisPtr = gameInstance->GetItemFileHandler(); // 만약을 대비해 Weak 캡처 추가
			httpHelper->Request(DA_NETWORK(ImgViewAddURL) + FString::Printf(TEXT("/%d/%d"), InItemId, InImgIdx), EHttpRequestType::GET,
				[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
								 {
									 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()) && InFunc)
									 {
										 // 파일 uTextrue2DDynamic으로 바꿔서 호출한다.
										 if (UTexture2DDynamic* texture = thisPtr.Pin()->_OnImgRequestCompleted(InResponse))
										 {
											 InFunc(texture);
										 }
									 }
									 else
									 {
										 LOG_ERROR(TEXT("이미지 파일 요청 실패"));
									 }
								 });
		}
	}
}


/**
 * glb 파일을 웹서버에 요청한다.
 * @param InFunc : 요청이 완료되면 실행할 람다 함수, 형식이 같아야 하며 람다 내부에서 클래스 멤버 접근시 weak 캡처 해주세요!
 * @param InItemId : 물품의 ItemId 
 */
void FItemFileHandler::_RequestGlbToWeb(const FCallbackRefOneParam<FString>& InFunc, uint32 InItemId) const
{
	// 없네. 요청
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 관련 파일을 요청한다.
		LOG_N(TEXT("Request Item ID (%d) glb Files ..."), InItemId);
		
		if (const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
		{
			TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetItemFileHandler(); // 만약을 대비해 Weak 캡처 추가
			httpHelper->Request(DA_NETWORK(GlbFileDownAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,
			[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							   {
								   if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								   {
									   // 파일을 저장한다.
									   FString fileSavedPath = thisPtr.Pin()->_OnGlbRequestCompleted(InResponse);

									   if (fileSavedPath != TEXT("") && InFunc)
									   {
										   InFunc(fileSavedPath);
										   return;
									   }
								   }
								   LOG_ERROR(TEXT("URL : %s, 파일 요청 실패"), *InRequest->GetURL());
							   });
		}
	}
}

/**
 * glb 파일 요청이 완료되면 호출될 함수, /Saved/Models 아래에 파일을 저장한다.
 * @param InResponse : http 응답
 * @return : 파일이 저장된 위치
 */
FString FItemFileHandler::_OnGlbRequestCompleted(const FHttpResponsePtr& InResponse) const
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
		return fileSavedPath;
	}
	
	LOG_ERROR(TEXT("file Handler is nullptr!"));
	return TEXT("");
}

/**
 * Img 파일 요청이 완료되면 호출될 함수, UTexture2DDynamic으로 바꾼다.
 * @param InResponse : http 응답
 */
UTexture2DDynamic* FItemFileHandler::_OnImgRequestCompleted(const FHttpResponsePtr& InResponse) const
{
	// ASyncTaskDownloadImage 긁어왔답니다 호호
	if ( InResponse.IsValid() && InResponse->GetContentLength() > 0 && InResponse->GetContent().Num() > 0 )
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		for ( auto ImageWrapper : ImageWrappers )
		{
			if ( ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InResponse->GetContent().GetData(), InResponse->GetContent().Num()) )
			{
				TArray64<uint8> RawData;
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if ( ImageWrapper->GetRaw(InFormat, 8, RawData) )
				{
					if ( UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()) )
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
						if (TextureResource)
						{
							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
								[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
								{
									TextureResource->WriteRawToTexture_RenderThread(RawData);
								});
						}
						return Texture;
					}
				}
			}
		}
	}

	return nullptr;
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
