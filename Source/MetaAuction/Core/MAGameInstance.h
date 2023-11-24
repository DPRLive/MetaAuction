// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/UserShareData.h"

#include <Engine/GameInstance.h>
#include "MAGameInstance.generated.h"

class FLoginData;

/**
 * Meta Auction에서 사용할 GameInstance
 */
UCLASS()
class METAAUCTION_API UMAGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMAGameInstance();

	// 게임이 시작될 때 호출됨 ( 에디터의 경우 시작 버튼을 눌렀을 때 )
	virtual void Init() override;

	// 게임이 종료될 때마다 호출됨 ( 에디터의 경우 종료 버튼을 눌렀을 때 )
	virtual void Shutdown() override;

	// 웹서버에 사용자 로그인을 요청하는 함수
	void RequestLogin(const FString& InID, const FString& InPassword);
	
	// LoginData Getter
	FORCEINLINE const TSharedPtr<FLoginData>& GetLoginData() const { return LoginData; }

	// Temp User Share Data Getter, PlayerState에서 레벨 이동시 데이터 유지용으로만 사용하세요!
	FORCEINLINE const FUserShareData& GetTempUserShareData() const { return TempUserShareData; }
	
	// ModelHandler Getter
	FORCEINLINE const TSharedPtr<FItemFileHandler>& GetItemFileHandler() const { return ItemFileHandler; }

	// Chat Handler Getter
	FORCEINLINE TObjectPtr<UChatHandler> GetChatHandler() const { return ChatHandler; }
	
	// HttpHelper Getter
	FORCEINLINE const TSharedPtr<FHttpHelper>& GetHttpHelper() const { return HttpHelper; }
	
	// StompHelper Getter
	FORCEINLINE const TSharedPtr<FStompHelper>& GetStompHelper() const { return StompHelper; }

private:
	// OnNotifyPreClientTravel에 등록되어, 레벨 이동이 일어나기 전 나의 UserData를 저장합니다. 
	void _SaveMyUserShareData(const FString& String, ETravelType Travel, bool Cond);
	
	// 로그인 관련 정보를 들고있기 위한 LoginData
	TSharedPtr<FLoginData> LoginData;

	// Client Travel시 유지할 사용할 임시 UserData 입니다. PlayerState에서 레벨 이동시 데이터 유지용으로만 사용하세요!
	UPROPERTY(Transient)
	FUserShareData TempUserShareData;
	
	// 아이템 관련 파일 처리를 위한 ItemFileHandler, 클라이언트에서만 생성됩니다.
	TSharedPtr<FItemFileHandler> ItemFileHandler;
	
	// Chat 관련 처리를 위한 Chat Handler
	UPROPERTY()
	TObjectPtr<UChatHandler> ChatHandler;
	
	// HTTP 통신을 위한 HttpHelper
	TSharedPtr<FHttpHelper> HttpHelper;

	// WebSocket 통신을 위한 StompHelper
	TSharedPtr<FStompHelper> StompHelper;
};
