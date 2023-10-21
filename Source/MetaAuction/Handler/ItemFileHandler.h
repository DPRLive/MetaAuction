#pragma once

#include <blueprint/AsyncTaskDownloadImage.h>

class UTexture2DDynamic;

/**
 * 모델링 파일 + 사진 파일을 관리하기 위한 ModelHandler 입니다.
 * 클라이언트에서만 생성되고 사용합니다.
 */
class FItemFileHandler
{
public:
	FItemFileHandler();
	
	// 캐시 파일들 (Saved/Models/에 저장되는 모델 파일들)을 지웁니다.
	void RemoveCacheFile(ERemoveCacheType InRemoveCacheType);
	
	// 해당 item ID의 파일들을 지웁니다.
	void RemoveGlbFile(uint32 InItemId) const;

	// 해당 item ID의 모델링 파일(glb)를 요청합니다.
	void RequestGlb(FCallbackOneParam<const FString&> InFunc, uint32 InItemId) const;

	// 해당 item Id의 index번째 이미지를 요청합니다.
	void RequestImg(FCallbackOneParam<UTexture2DDynamic*> InFunc, uint32 InItemId, uint8 InImgIdx);
private:
	// glb 파일 요청이 완료되면 호출될 함수, 파일을 저장한다.
	void _OnGlbRequestCompleted(const FHttpResponsePtr& InResponse) const;

	// Img 파일 요청이 완료되면 호출될 함수, UTexture2DDynamic으로 바꾼다.
	UTexture2DDynamic* _OnImgRequestCompleted(const FHttpResponsePtr& InResponse) const;
	
	// 헤더에서 ""을 기준으로 컨텐츠를 뽑아내주는 함수
	FString _ParseInHeader(const FString& InContent) const;
};

