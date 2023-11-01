#include "StompHelper.h"

#include <Modules/ModuleManager.h>
#include <StompModule.h>

/**
 *  Stomp WebSocket에 연결하고 기능을 수행하는 handler
 */
FStompHelper::FStompHelper()
{
	// 모듈이 로드가 안되었다면 로드 시킴
	if(!FModuleManager::Get().IsModuleLoaded(TEXT("Stomp")))
	{
		FModuleManager::Get().LoadModule(TEXT("Stomp"));
	}

	// Stomp 객체를 연결한다.
	Stomp = FStompModule::Get().CreateClient(DA_NETWORK(WSServerURL));
	Stomp->OnConnected().AddRaw(this, &FStompHelper::_OnConnectedSuccess);
	Stomp->Connect();
}

/**
 *  해당 url에 STOMP WebSocket 구독을 요청합니다.
 *  @param InUrl : sub할 Url
 *  @param InEvent : broadcast 받을 event
*/
void FStompHelper::Subscribe(const FString& InUrl, const FStompSubscriptionEvent& InEvent) const
{
	if(Stomp.IsValid() && Stomp->IsConnected())
	{
		Stomp->Subscribe(InUrl, InEvent);
	}
}

/**
 *  해당 Websocket Url로 메세지를 보낸다
 *  @param InUrl : pub할 url
 *  @param InContent : 보낼 Json 형태의 message
 */
void FStompHelper::SendMessage(const FString& InUrl, const FString& InContent) const
{
	if(Stomp.IsValid() && Stomp->IsConnected())
	{
		Stomp->Send(InUrl, InContent);
	}
}

/**
 *  연결 성공시 호출될 함수
 */
void FStompHelper::_OnConnectedSuccess(const FString& InProtocolVersion, const FString& InSessionId, const FString& InServerString) const
{
	LOG_N(TEXT("Stomp Web Socket Connected!"));
	LOG_N(TEXT("ProtocolVersion : %s"), *InProtocolVersion);
	LOG_N(TEXT("SessionID : %s"), *InSessionId);
	LOG_N(TEXT("ServerString : %s"), *InServerString);
}


