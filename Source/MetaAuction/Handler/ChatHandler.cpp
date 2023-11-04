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
FDelegateHandle UChatHandler::SubscribeItemReply(const uint32 InItemId, const FCallbackRefOneParam<FChatData>& InFunc)
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
				FChatData reply;
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
 *  id로 채팅 or 댓글들을 요청합니다.
 *  InChatType이 Chatroom일 경우 : id는 채팅방 id로 사용합니다.
 *  InChatType이 ItemReply일 경우 : id는 물품 id로 사용합니다.
 *  Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
 *  @param InChatType : 채팅 요청 타입, 채팅방 or 물품 댓글
 *  @param InId : InChatType이 Chatroom(id는 채팅방 id로 사용), InChatType이 ItemReply일 경우(id는 물품 id로 사용)
 *  @param InFunc : 채팅 or 댓글을 수신하면 실행할 람다함수, const TArray<FChatData>&를 인자로 받아야함
 */
void UChatHandler::RequestChatsById(const ERequestChatType InChatType, const uint32 InId, const FCallbackRefArray<FChatData>& InFunc) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		FString ReqUrl;
		if(InChatType == ERequestChatType::ItemReply)
		{
			ReqUrl = DA_NETWORK(ItemReplyAddURL) + FString::Printf(TEXT("/%d"), InId);
			LOG_N(TEXT("Request Item ID (%d) Replies ..."), InId);
		}
		else if(InChatType == ERequestChatType::Chatroom)
		{
			ReqUrl = DA_NETWORK(ChatroomChatAddURL) + FString::Printf(TEXT("/%d"), InId);
			LOG_N(TEXT("Chatroom ID (%d) Chats ..."), InId);
		}

		// HTTP 통신으로 댓글들을 요청한다.
		TWeakObjectPtr<const UChatHandler> thisPtr = this;
		httpHelper->Request(ReqUrl, EHttpRequestType::GET,
					[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
						   {
							   if (thisPtr.IsValid() && InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
							   {
								   TArray<TSharedPtr<FJsonObject>> jsonArray;
								   UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

								   TArray<FChatData> replyArray;
								   for (const TSharedPtr<FJsonObject>& jsonObj : jsonArray)
								   {
									   replyArray.Emplace(FChatData());
									   thisPtr->_JsonToData(jsonObj, *replyArray.rbegin());
								   }

								   InFunc(replyArray);
								   return;
							   }
							   LOG_ERROR(TEXT("URL : %s, 댓글 or 채팅 요청 실패"), *InRequest->GetURL());
						   });
		
	}
}

/**
 * 나의 채팅방 목록을 모두 불러옵니다. (로그인 된 상태에서만 사용 가능)
 * Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
 * @param InFunc : 채팅방들을 수신하면 실행할 람다함수, const TArray<FItemReply>&를 인자로 받아야함
 */
void UChatHandler::RequestMyChatRoom(const FCallbackRefArray<FChatRoomData>& InFunc) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 채팅방들을 요청한다.
		LOG_N(TEXT("Request my chatrooms ..."));
		
		TWeakObjectPtr<const UChatHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(MyChatRoomAddURL), EHttpRequestType::GET,
		[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
					   {
						   if (thisPtr.IsValid() && InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
						   {
							   TArray<TSharedPtr<FJsonObject>> jsonArray;
							   UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

							   TArray<FChatRoomData> chatRoomArray;
							   for (const TSharedPtr<FJsonObject>& jsonObj : jsonArray)
							   {
								   chatRoomArray.Emplace(FChatRoomData());
								   thisPtr->_JsonToData(jsonObj, *chatRoomArray.rbegin());
							   }
						   	
							   InFunc(chatRoomArray);
							   return;
						   }
						   LOG_ERROR(TEXT("URL : %s, 내 채팅방 요청 실패"), *InRequest->GetURL());
					   });
	}
}

/**
 * Json 형태의 ItemReply String을 FItemReply로 변환한다.
 * @param InJsonObj : Json 형태의 ItemReply String
 * @param OutItemReply : 파싱한 정보를 담아갈 FItemReply
 */
void UChatHandler::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatData& OutItemReply) const
{
	InJsonObj->TryGetStringField(TEXT("sender"), OutItemReply.Sender);
	InJsonObj->TryGetStringField(TEXT("text"), OutItemReply.Content);
	// 시간을 파씽
	FString timeString;
	InJsonObj->TryGetStringField(TEXT("messageTime"), timeString);
	FDateTime::ParseIso8601(*timeString, OutItemReply.Time);
}

/**
 * Json 형태의 ChatRoomData JsonObject를 FChatRoomData 변환한다.
 * @param InJsonObj : Json 형태의 ChatRoomData String
 * @param OutChatRoomData : 파싱한 정보를 담아갈 FChatRoomData
 */
void UChatHandler::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatRoomData& OutChatRoomData) const
{
	InJsonObj->TryGetNumberField(TEXT("id"), OutChatRoomData.ChatRoomId);
	InJsonObj->TryGetNumberField(TEXT("itemId"), OutChatRoomData.ItemId);
	InJsonObj->TryGetStringField(TEXT("seller"), OutChatRoomData.Seller);
	InJsonObj->TryGetStringField(TEXT("buyer"), OutChatRoomData.Buyer);
}
