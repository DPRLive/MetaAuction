// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/GameInstance.h>
#include "MAGameInstance.generated.h"

class FItemFileHandler;
class FStompHandler;

// 로그인 관련 정보를 담는 struct
struct FLoginData
{
	FLoginData() : bLogin(false), JwtToken(TEXT("")) {}

	// 로그인이 된 상태인지
	uint8 bLogin:1;
	
	// JWT 토큰
	FString JwtToken;
};

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
	FORCEINLINE const FLoginData& GetLoginData() const { return LoginData; }

	// ModelHandler Getter
	FORCEINLINE const TSharedPtr<FItemFileHandler>& GetItemFileHandler() const { return ItemFileHandler; }

	// HttpHandler Getter
	FORCEINLINE const TSharedPtr<FHttpHandler>& GetHttpHandler() const { return HttpHandler; }
	
	// StompHandler Getter
	FORCEINLINE const TSharedPtr<FStompHandler>& GetStompHandler() const { return StompHandler; }
private:
	// 로그인 관련 정보를 들고있기 위한 LoginData
	FLoginData LoginData;

	// 아이템 관련 파일 처리를 위한 ItemFileHandler, 클라이언트에서만 생성됩니다.
	TSharedPtr<FItemFileHandler> ItemFileHandler;

	// HTTP 통신을 위한 HttpHandler
	TSharedPtr<FHttpHandler> HttpHandler;

	// WebSocket 통신을 위한 Stomp Handler
	TSharedPtr<FStompHandler> StompHandler;
};
