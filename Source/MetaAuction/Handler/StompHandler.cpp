#include "StompHandler.h"
#include "../core/MAGameMode.h"
#include "Core/MAGameState.h"
#include "..\Manager\ItemManager.h"

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

	// New Item용 구독
	OnNewItem.BindRaw(this, &FStompHandler::_OnNewItem);
	Stomp->Subscribe(DA_NETWORK(NewItemAddURL), OnNewItem);	
}

/**
 *  New Item 알림 Subscribe 한 곳에 이벤트 도착시 호출될 함수
 */
void FStompHandler::_OnNewItem(const IStompMessage& InMessage)
{
	uint32 itemID = FCString::Atoi(*InMessage.GetBodyAsString());

	LOG_N(TEXT("Server : On New Item %d!"), itemID);
	
	// Item Manager에게 새로운 아이템 등록을 요청한다.
	if(AMAGameState* gameState = Cast<AMAGameState>(MAGetWorld()->GetGameState()))
	{
		if(gameState->GetItemManager() != nullptr)
		{
			gameState->GetItemManager()->Server_RequestItemDataAndRegister(itemID);
		}
	}
}

/**
 *  서버로 메세지를 보낸다
 */
void FStompHandler::SendMessage()
{
	// Stomp->Send(TEXT("/sub/test"), TEXT("{\"unreal\" : \"hello\"}"));
}