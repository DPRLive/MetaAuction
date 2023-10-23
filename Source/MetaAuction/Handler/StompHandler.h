#pragma once

#include <IStompClient.h>

/**
 *  Stomp WebSocket에 연결하고 기능을 수행하는 handler
 * 
 */
class FStompHandler
{
public:
	// 생성자
	FStompHandler();
	
	// 서버로 메세지를 보낸다
	void SendMessage();
	
private:
	// 연결 성공시 호출될 함수
	void _OnConnectedSuccess(const FString& InProtocolVersion, const FString& InSessionId, const FString& InServerString);

public:
	// 해당 url에 STOMP WebSocket 구독을 요청합니다.
	void Subscribe(const FString& InUrl, const FStompSubscriptionEvent& InEvent);

private:
	// Stomp 객체
	TSharedPtr<IStompClient> Stomp;
};
