#pragma once

#include <IStompClient.h>

/**
 *  Stomp WebSocket에 연결하고 기능을 수행하는 Helper
 * 
 */
class FStompHelper
{
public:
	// 생성자
	FStompHelper();

	// 해당 url에 STOMP WebSocket 구독을 요청합니다.
	FStompSubscriptionId Subscribe(const FString& InUrl, const FStompSubscriptionEvent& InEvent) const;

	// 해당 Event Id의 STOMP sub Event 구독을 해제한다.
	void Unsubscribe(const FStompSubscriptionId& InEventId) const;
	
	// 해당 Websocket Url로 메세지를 보낸다
	void SendMessage(const FString& InUrl, const FString& InContent) const;
	
private:
	// 연결 성공시 호출될 함수
	void _OnConnectedSuccess(const FString& InProtocolVersion, const FString& InSessionId, const FString& InServerString) const;
	
	// Stomp 객체
	TSharedPtr<IStompClient> Stomp;
};
