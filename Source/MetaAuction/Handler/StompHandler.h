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

	// New Item 알림 Subscribe 한 곳에 이벤트 도착시 호출될 함수
	void _Server_OnNewItem(const IStompMessage& InMessage) const;

	// stomp 메세지로 온 item을 지워버린다
	void _Server_OnRemoveItem(const IStompMessage& InMessage) const;
	
	// Stomp 객체
	TSharedPtr<IStompClient> Stomp;
	
	// New Item 알림 Subscribe 한 곳에 이벤트 도착시 호출, 서버에서 사용
	FStompSubscriptionEvent Server_OnNewItem;

	// Remove Item 알림 Subscribe 한 곳에 이벤트 도착시 호출, 서버에서 사용
	FStompSubscriptionEvent Server_OnRemoveItem;
};
