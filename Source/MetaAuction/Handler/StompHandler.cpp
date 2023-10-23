#include "StompHandler.h"

#include <Modules/ModuleManager.h>
#include <StompModule.h>
#include <IStompMessage.h>

/**
 *  Stomp WebSocket에 연결하고 기능을 수행하는 handler
 */
FStompHandler::FStompHandler()
{
	// 모듈이 로드가 안되었다면 로드 시킴
	if(!FModuleManager::Get().IsModuleLoaded(TEXT("Stomp")))
	{
		FModuleManager::Get().LoadModule(TEXT("Stomp"));
	}

	// Stomp 객체를 연결한다.
	Stomp = FStompModule::Get().CreateClient(DA_NETWORK(WSServerURL));
	Stomp->OnConnected().AddRaw(this, &FStompHandler::_OnConnectedSuccess);
	Stomp->Connect();
}

/**
 *  연결 성공시 호출될 함수
 */
void FStompHandler::_OnConnectedSuccess(const FString& InProtocolVersion, const FString& InSessionId, const FString& InServerString)
{
	LOG_N(TEXT("Stomp Web Socket Connected!"));
	LOG_N(TEXT("ProtocolVersion : %s"), *InProtocolVersion);
	LOG_N(TEXT("SessionID : %s"), *InSessionId);
	LOG_N(TEXT("ServerString : %s"), *InServerString);
}

/**
 *  해당 url에 STOMP WebSocket 구독을 요청합니다.
 */
void FStompHandler::Subscribe(const FString& InUrl, const FStompSubscriptionEvent& InEvent)
{
	if(Stomp.IsValid())
	{
		Stomp->Subscribe(InUrl, InEvent);
	}
}

/**
 *  서버로 메세지를 보낸다
 */
void FStompHandler::SendMessage()
{
	// Stomp->Send(TEXT("/sub/test"), TEXT("{\"unreal\" : \"hello\"}"));
}


