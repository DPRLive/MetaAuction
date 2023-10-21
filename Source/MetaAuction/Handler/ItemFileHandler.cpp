﻿#include "ItemFileHandler.h"
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

/**
 * 생성자
 */
FItemFileHandler::FItemFileHandler()
{
}

/**
 * 캐시 파일들 (Saved/Models/에 저장되는 모델 파일들)을 지웁니다.
 * 아마 프로그램 실행 시 메인 화면에서 로그인 후 레벨 넘어가기 전 unuseable로 한번 호출해주면 좋을듯해요
 * @param InRemoveCacheType : 파일을 지울때의 옵션, Unuseable일 경우 로그인 된 상태에서 해주세요.
 */
void FItemFileHandler::RemoveCacheFile(ERemoveCacheType InRemoveCacheType)
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
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		httpHandler->Request(DA_NETWORK(ItemSearchAddURL), EHttpRequestType::POST,[basePath](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // Json reader 생성
									 TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
									 TArray<TSharedPtr<FJsonValue>> jsonValues;
									 FJsonSerializer::Deserialize(reader, jsonValues);

								 	// 지우지 않아야 할 물품 번호들을 뽑는다
									 TSet<uint32> sellItemIds;
									 for (TSharedPtr<FJsonValue>& itemInfo : jsonValues)
									 {
										 TSharedPtr<FJsonObject> itemInfoObj = itemInfo->AsObject();
										 uint32 id;
										 if(itemInfoObj->TryGetNumberField(TEXT("id"), id))
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
							 }, httpHandler->JsonToString(requestObj));
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
 * 해당 item ID의 모델링 파일(glb)를 요청합니다.
 * @param InFunc : 요청이 완료되면 실행할 람다 함수, 형식이 같아야 하며 람다 내부에서 클래스 멤버 접근시 weak 캡처 해주세요!
 * @param InItemId : 물품의 ItemId 
 */
void FItemFileHandler::RequestGlb(FCallbackOneParam<const FString&> InFunc, uint32 InItemId) const
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
			TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetItemFileHandler(); // 만약을 대비해 Weak 캡처 추가
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
 * 해당 item Id의 index번째 이미지를 요청합니다.
 * @param InFunc : 요청이 완료되면 실행할 람다 함수, 형식이 같아야 하며 람다 내부에서 클래스 멤버 접근시 weak 캡처 해주세요! 
 * @param InItemId : 요청할 Item의 id
 * @param InImgIdx : 요청할 Item의 몇번째 사진인지?
 */
void FItemFileHandler::RequestImg(FCallbackOneParam<UTexture2DDynamic*> InFunc, uint32 InItemId, uint8 InImgIdx)
{
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(MAGetGameInstance()))
	{
		// HTTP 통신으로 관련 파일을 요청한다.
		LOG_N(TEXT("Request Item ID (%d) Img File (%d) ..."), InItemId, InImgIdx);
		
		if (UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
		{
			TWeakPtr<FItemFileHandler> thisPtr = gameInstance->GetItemFileHandler(); // 만약을 대비해 Weak 캡처 추가
			httpHandler->Request(DA_NETWORK(ImgViewAddURL) + FString::Printf(TEXT("/%d/%d"), InItemId, InImgIdx), EHttpRequestType::GET,
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
 * glb 파일 요청이 완료되면 호출될 함수, /Saved/Models 아래에 파일을 저장한다.
 * @param InResponse : http 응답
 */
void FItemFileHandler::_OnGlbRequestCompleted(const FHttpResponsePtr& InResponse) const
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
