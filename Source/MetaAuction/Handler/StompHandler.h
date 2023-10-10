#pragma once

#include <IStompClient.h>

class AMAGameMode;
/**
 *  Stomp WebSocket에 연결하고 기능을 수행하는 handler
 *  GameMode에 두어 Dedicated Server에서만 사용 예정
 */
class FStompHandler
{
public:
	// 생성자
	FStompHandler();

private:
	// 연결 성공시 호출될 함수
	void _OnConnectedSuccess(const FString& InProtocolVersion, const FString& InSessionId, const FString& InServerString);

	// New Item 알림 Subscribe 한 곳에 이벤트 도착시 호출될 함수
	void _OnNewItem(const IStompMessage& InMessage);

public:
	// 서버로 메세지를 보낸다
	void SendMessage();
	
private:
	// Stomp 객체
	TSharedPtr<IStompClient> Stomp;
	
	// New Item 알림 Subscribe 한 곳에 이벤트 도착시 호출
	FStompSubscriptionEvent OnNewItem;
};
