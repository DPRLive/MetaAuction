// Fill out your copyright notice in the Description page of Project Settings.


#include "Handler/ChatHandler.h"
#include "Data/LoginData.h"
#include "Core/MAGameInstance.h"

#include <Dom/JsonObject.h>
#include <IStompMessage.h>
#include <Interfaces/IHttpResponse.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ChatHandler)

UChatHandler::UChatHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChatHandler::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
 *  @param InItemId : 댓글을 달 item id
 *  @param InContent : 댓글 내용 
 */
void UChatHandler::AddReplyToItem(const uint32 InItemId, const FString& InContent) const
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
 *  @param InFunc : 댓글을 수신하면 실행할 람다함수, const FItemReply&를 인자로 받아야함
 *  @return : 해당 Lambda를 바인드 후 리턴된 Delegate Handle
 */
FDelegateHandle UChatHandler::SubscribeItemReply(const uint32 InItemId, const FCallbackRefOneParam<FItemReply>& InFunc)
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

		TWeakObjectPtr<UChatHandler> thisPtr = this;
		replyEvent.BindLambda([thisPtr, InItemId](const IStompMessage& InMessage)
		{
			auto it = thisPtr->ItemReplyDelegates.Find(InItemId);
			
			if(thisPtr.IsValid() && it != nullptr && it->Value.IsValid() && it->Value->IsBound())
			{
				TSharedPtr<FJsonObject> replyObj = UtilJson::StringToJson(InMessage.GetBodyAsString());

				// 파싱 후
				FItemReply reply;
				thisPtr->_JsonToData(replyObj, reply);

				// 뿌려준다
				it->Value->Broadcast(reply);
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
void UChatHandler::UnsubscribeItemReply(const uint32 InItemId, const FDelegateHandle& InHandle)
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

/**
 *  해당 물품 id에 달린 댓글을 모두 요청합니다.
 *  Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
 *  @param InItemId : 댓글을 가져올 대상 item id
 *  @param InFunc : 댓글을 수신하면 실행할 람다함수, const TArray<FItemReply>&를 인자로 받아야함
 */
void UChatHandler::RequestItemReply(const uint32 InItemId, const FCallbackRefArray<FItemReply>& InFunc)
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 댓글들을 요청한다.
		LOG_N(TEXT("Request Item ID (%d) Replies ..."), InItemId);
		
		TWeakObjectPtr<UChatHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(ItemReplyAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,
					[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
						   {
							   if (thisPtr.IsValid() && InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
							   {
								   TArray<TSharedPtr<FJsonObject>> jsonArray;
								   UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

								   TArray<FItemReply> replyArray;
								   for (const TSharedPtr<FJsonObject>& jsonObj : jsonArray)
								   {
									   replyArray.Emplace(FItemReply());
									   thisPtr->_JsonToData(jsonObj, *replyArray.rbegin());
								   }

								   InFunc(replyArray);
								   return;
							   }
							   LOG_ERROR(TEXT("URL : %s, 댓글 요청 실패"), *InRequest->GetURL());
						   });
		
	}
}

/**
 * Json 형태의 ItemReply String을 FItemReply로 변환한다.
 * @param InJsonObj : Json 형태의 ItemReply String
 * @param OutItemReply : 파싱한 정보를 담아갈 FItemReply
 */
void UChatHandler::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FItemReply& OutItemReply) const
{
	InJsonObj->TryGetStringField(TEXT("sender"), OutItemReply.Sender);
	InJsonObj->TryGetStringField(TEXT("text"), OutItemReply.Content);
	// 시간을 파씽
	FString timeString;
	InJsonObj->TryGetStringField(TEXT("messageTime"), timeString);
	FDateTime::ParseIso8601(*timeString, OutItemReply.Time);
}
