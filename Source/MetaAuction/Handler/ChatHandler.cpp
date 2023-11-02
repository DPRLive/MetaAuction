#include "ChatHandler.h"
#include "Data/LoginData.h"
#include "Core/MAGameInstance.h"

#include <Dom/JsonObject.h>
#include <IStompMessage.h>

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

/**
 *  Stomp로 해당 item ID에 달리는 댓글을 수신합니다. 여러함수를 바인드 할 수 있습니다.
 *  댓글 수신이 필요 없어지면 (상세 정보 창을 닫는다던가)
 *  꼭 리턴받은 FDelegateHandle로 해당 함수를 UnsubscribeItemReply 해주세요! 메모리 누수가 발생할 수 있습니다.
 *  Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
 *  @param InItemId : 댓글을 수신할 대상 item id
 *  @param InFunc : 댓글을 수신하면 실행할 람다함수
 *  @return : 해당 Lambda를 바인드 후 리턴된 Delegate Handle
 */
FDelegateHandle FChatHandler::SubscribeItemReply(const uint32 InItemId, FCallbackOneParam<const FItemReply&> InFunc)
{
	// 이미 이 item id의 댓글을 수신 받고 있음
	if(FReplyDelegateInfo* replyDelegate = ItemReplyDelegates.Find(InItemId))
	{
		return replyDelegate->Value->AddLambda(InFunc);
	}
 
	// 아니면 새로 만든다
	TSharedPtr<FOnItemReply> replyDelegate = MakeShareable(new FOnItemReply());
	FDelegateHandle delegateHandle = replyDelegate->AddLambda(InFunc);

	if(const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
	{
		FStompSubscriptionEvent replyEvent;
		TWeakPtr<FOnItemReply> replyDelegateWeak = replyDelegate;
		replyEvent.BindLambda([replyDelegateWeak](const IStompMessage& InMessage)
		{
			if (replyDelegateWeak.IsValid() && replyDelegateWeak.Pin()->IsBound())
			{
				TSharedPtr<FJsonObject> replyObj = UtilJson::StringToJson(InMessage.GetBodyAsString());

				FItemReply reply;
				replyObj->TryGetStringField(TEXT("sender"), reply.Sender);
				replyObj->TryGetStringField(TEXT("text"), reply.Content);

				// 시간을 파씽
				FString timeString;
				replyObj->TryGetStringField(TEXT("messageTime"), timeString);
				FDateTime::ParseIso8601(*timeString, reply.Time);

				// 댓글을 뿌려준다.
				replyDelegateWeak.Pin()->Broadcast(reply);
			}
		});

		const FStompSubscriptionId& stompId = stompHandler->Subscribe(DA_NETWORK(WSReceiveChatAddURL) + FString::Printf(TEXT("/%d"), InItemId), replyEvent);
		ItemReplyDelegates.Emplace(InItemId, FReplyDelegateInfo(stompId, replyDelegate));
	}

	return delegateHandle;
}

/**
 *  Stomp로 해당 item ID에 달리는 댓글을 수신하던 함수를 delegate에서 해제합니다.
 *  @param InItemId : 대상 item id
 *  @param InHandle : 댓글 수신을 중지할 함수의 Delegate handle
 */
void FChatHandler::UnsubscribeItemReply(const uint32 InItemId, const FDelegateHandle& InHandle)
{
	// delegate 나와라
	if(FReplyDelegateInfo* replyDelegate = ItemReplyDelegates.Find(InItemId))
	{
		// 해당 함수를 바인드 해제
		replyDelegate->Value->Remove(InHandle);

		if(replyDelegate->Value->IsBound()) 
			return;
		
		// 만약 더 이상 바인드 된 함수가 없다면? websocket 구독도 해제
		if (const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
		{
			stompHandler->Unsubscribe(replyDelegate->Key);
		}

		// Map에서도 삭제
		ItemReplyDelegates.Remove(InItemId);
	}
}
