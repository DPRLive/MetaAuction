#pragma once

/**
 * 모델링 파일 + 사진 파일을 관리하기 위한 ModelHandler 입니다.
 * 클라이언트에서만 생성되고 사용합니다.
 */
class FItemFileHandler
{
public:
	FItemFileHandler();

	// 모델링 파일 또는 사진 파일이 모두 다운되면, 다운 된 경로를 delegate를 통해 알린다.
	// 아이템 id, 모델 파일 경로
	// 사용시 여러 item actor에게 알림이 갈 수 있으니 아이템 id를 체크하고, 사용 후 remove bind 할 것
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCompletedModelReq, uint32, const FString&)
	FOnCompletedModelReq CompletedItemFileReq;
	
	// 필요한 파일들을 요청하는 함수
	void RequestItemFiles(uint32 InItemID, uint8 InFileCount) const;

	// 해당 item ID의 파일들을 지웁니다.
	void RemoveItemFiles(uint32 InItemID);

private:
	// 파일 요청이 완료되면 호출될 함수, 파일을 저장한다.
	void _OnFileRequestCompleted(uint32 InItemID, FHttpResponsePtr InResponse) const;

	// 헤더에서 ""을 기준으로 컨텐츠를 뽑아내주는 함수
	FString _ParseInHeader(const FString& InContent) const;
};
