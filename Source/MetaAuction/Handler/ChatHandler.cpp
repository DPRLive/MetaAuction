#include "ChatHandler.h"
#include "Data/LoginData.h"
#include "Core/MAGameInstance.h"

#include <Dom/JsonObject.h>

/**
 *  Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
 *  @param InItemId : 댓글을 달 item id
 *  @param InContent : 댓글 내용 
 */
void FChatHandler::AddReplyToItem(const uint32 InItemId, const FString& InContent) const
{
	// Json object를 통해 내용을 json 형식으로 만든다.
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	if(const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance(MAGetWorld())))
	{
		const TWeakPtr<FLoginData> loginData = gameInstance->GetLoginData(); 
		if(loginData.IsValid())
		{
			requestObj->SetStringField(TEXT("sender"), loginData.Pin()->GetUserName());
			requestObj->SetStringField(TEXT("text"), InContent);
		}
	}

	// 전송
	FString pubUrl = DA_NETWORK(WSSendChatAddURL) + FString::Printf(TEXT("/%d"), InItemId);
	if(const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
	{
		stompHandler->SendMessage(pubUrl, UtilJson::JsonToString(requestObj));
	}
}