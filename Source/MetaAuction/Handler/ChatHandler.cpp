// Fill out your copyright notice in the Description page of Project Settings.


#include "Handler/ChatHandler.h"
#include "Core/MAGameInstance.h"

#include <Dom/JsonObject.h>
#include <IStompMessage.h>
#include <Interfaces/IHttpResponse.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ChatHandler)

UChatHandler::UChatHandler()
{
}

/**
 *  ChatHandler를 초기화 합니다.
 */
void UChatHandler::InitChatHandler()
{
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
	{
		gameInstance->OnLoginDelegate.AddUObject(this, &UChatHandler::_AfterLogin);
	}
}

/**
 *  로그인 이후 로직을 처리합니다.
 */
void UChatHandler::_AfterLogin(bool InbSuccess)
{
	if(!InbSuccess)
		return;
	
	LOG_N(TEXT("Prepare ChatHandler..."));
	
	// 나의 채팅방을 모두 등록
	RequestMyChatRoom([](const TArray<TPair<FChatRoomData, FChatData>>& InChatRoomDatas){ });
	
	// 나에게 생길 채팅방도 생길때마다 등록 할 수 있도록 함
	if (const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
	{
		FStompSubscriptionEvent subEvent;

		TWeakObjectPtr<UChatHandler> thisPtr = this;
		subEvent.BindLambda([thisPtr](const IStompMessage& InMessage)
		{
			if(thisPtr.IsValid())
			{
				// ChatRoomId를 뽑아낸다
				TSharedPtr<FJsonObject> jsonObject = UtilJson::StringToJson(InMessage.GetBodyAsString());
				uint32 chatRoomId;
				if(jsonObject->TryGetNumberField(TEXT("chatRoomId"), chatRoomId))
				{
					thisPtr->_TrySubscribe1On1Chat(chatRoomId);	
				}
			}
		});

		FString subUrl = DA_NETWORK(WSReceiveNewChatAddURL).Replace(TEXT("%s"), *MAGetMyUserName(MAGetGameInstance()));
		stompHandler->Subscribe(subUrl, subEvent);
	}
}

/**
 *  Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
 *  @param InItemId : 댓글을 달 item id
 *  @param InContent : 댓글 내용 
 */
void UChatHandler::AddReplyToItem(const uint32 InItemId, const FString& InContent) const
{
	const FString userName = MAGetMyUserName(MAGetGameInstance());
	if(userName == TEXT(""))
		return;
	
	// Json object를 통해 내용을 json 형식으로 만든다.
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	requestObj->SetStringField(TEXT("sender"), userName);
	requestObj->SetStringField(TEXT("text"), InContent);

	// 전송
	FString pubUrl = DA_NETWORK(WSSendChatAddURL) + FString::Printf(TEXT("/%d"), InItemId);
	if (const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
	{
		stompHandler->SendMessage(pubUrl, UtilJson::JsonToString(requestObj));
	}
}

/**
 *  Stomp로 InChatRoomDta에 맞는 채팅방에 1대1 거래 채팅을 보낸다. 로그인 된 경우만 사용 가능
 *  @param InChatRoomData : 채팅을 보낼 대상 채팅방의 정보
 *  @param InContent : 보낼 내용
 */
void UChatHandler::Send1On1Chat(const FChatRoomData& InChatRoomData, const FString& InContent) const
{
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();

	// 내가 buyer인지 seller인지 확인
	const FString userName = MAGetMyUserName(MAGetGameInstance());
	if(userName == InChatRoomData.Buyer)
		requestObj->SetStringField(TEXT("whoAmI"), TEXT("buyer"));
	else if(userName == InChatRoomData.Seller)
		requestObj->SetStringField(TEXT("whoAmI"), TEXT("seller"));
	else
		return;
	
	// Json object를 통해 내용을 json 형식으로 만든다.
	requestObj->SetStringField(TEXT("buyer"), InChatRoomData.Buyer);
	requestObj->SetStringField(TEXT("seller"), InChatRoomData.Seller);
	requestObj->SetStringField(TEXT("text"), InContent);

	// 전송
	FString pubUrl = DA_NETWORK(WSSendChatAddURL) + FString::Printf(TEXT("/%d"), InChatRoomData.ItemId);
	if (const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
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
	TSharedPtr<FOnItemReplyDelegate> replyDelegate = MakeShareable(new FOnItemReplyDelegate());
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

		const FStompSubscriptionId& stompId = stompHandler->Subscribe(DA_NETWORK(WSReceiveReplyAddURL) + FString::Printf(TEXT("/%d"), InItemId), replyEvent);
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
 * @param InFunc : 채팅방들을 수신하면 실행할 람다함수, const TArray<TPair<FChatRoomData, FChatData>>&를 인자로 받아야함. (채팅방 정보, 해당 채팅방의 마지막 채팅의 정보)
 */
void UChatHandler::RequestMyChatRoom(const FCallbackRefArray<TPair<FChatRoomData, FChatData>>& InFunc)
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 채팅방들을 요청한다.
		LOG_N(TEXT("Request my chatrooms ..."));
		
		TWeakObjectPtr<UChatHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(MyChatRoomAddURL), EHttpRequestType::GET,
		[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
					   {
						   if (thisPtr.IsValid() && InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
						   {
							   TArray<TSharedPtr<FJsonObject>> jsonArray;
							   UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

							   TArray<TPair<FChatRoomData, FChatData>> chatRoomArray;
							   for (const TSharedPtr<FJsonObject>& jsonObj : jsonArray)
							   {
								   chatRoomArray.Emplace(FChatRoomData(), FChatData());
								   thisPtr->_JsonToData(jsonObj, chatRoomArray[chatRoomArray.Num() - 1].Key);
								   thisPtr->_JsonToData(jsonObj, chatRoomArray[chatRoomArray.Num() - 1].Value);
								   // 빠진게 있을 수 있으니 모두 구독을 시도해본다.
								   thisPtr->_TrySubscribe1On1Chat(chatRoomArray[chatRoomArray.Num() - 1].Key.ChatRoomId);
							   }
						   	
							   InFunc(chatRoomArray);
							   return;
						   }
						   LOG_ERROR(TEXT("URL : %s, 내 채팅방 요청 실패"), *InRequest->GetURL());
					   });
	}
}

/**
 * 새로운 채팅방을 생성합니다. 경매의 경우는 끝나면 자동으로 채팅방이 생성되니 일반 판매일 때만 사용하면 됩니다.
 * (특정 유저가 어떤 일반 판매 물품을 사고싶으면 이 api를 호출해서 채팅을 시작하면 됨)
 * Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
 * @param InItemId : 구매 채팅을 시작할 아이템의 id
 * @param InSellerName : 구매 채팅을 시작할 아이템을 파는 사람
 * @param InFunc : 채팅방이 성공적으로 만들어지면 호출할 함수
 */
void UChatHandler::RequestNewChatRoom(const uint32 InItemId, const FString& InSellerName, const FCallbackRefOneParam<FChatRoomData>& InFunc)
{
	FString userName = MAGetMyUserName(MAGetGameInstance());
	if(userName == TEXT(""))
		return;
	
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	requestObj->SetStringField(TEXT("buyer"), userName);
	requestObj->SetStringField(TEXT("seller"), InSellerName);
	
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(MAGetGameInstance()))
	{
		// HTTP 통신으로 새로운 채팅방을 생성한다.
		LOG_N(TEXT("Create New Chatrooms ..."));
		
		TWeakObjectPtr<UChatHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(CreateChatRoomAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::POST,
		[thisPtr, InItemId, InSellerName, userName, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
					   {
						   if (thisPtr.IsValid() && InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
						   {
							   FChatRoomData chatRoomData;
							   const uint32 chatRoomId = FCString::Atoi(*InResponse->GetContentAsString());
							   chatRoomData.ChatRoomId = chatRoomId;
							   chatRoomData.ItemId = InItemId;
							   chatRoomData.Seller = InSellerName;
							   chatRoomData.Buyer = userName;

							   // 채팅방을 구독한다
							   thisPtr->_TrySubscribe1On1Chat(chatRoomId);

							   // 뿌려준다
							   InFunc(chatRoomData);
							   return;
						   }
						   LOG_ERROR(TEXT("URL : %s, 새로운 채팅 요청 실패"), *InRequest->GetURL());
					   }, UtilJson::JsonToString(requestObj));
	}
}

/**
 * 웹소켓에 1대1 채팅을 Subscribe하여 특정 채팅방에 오는 채팅을 모두 받는다.
 * @param InChatRoomId : 구독할 채팅방의 id
 */
void UChatHandler::_TrySubscribe1On1Chat(const uint32 InChatRoomId)
{
	// 이미 구독된 채팅방이면 구독을 하지 않는다.
	if(Sub1On1ChatRooms.Find(InChatRoomId) != nullptr)
		return;
	
	if(const FStompHelper* stompHandler = MAGetStompHelper(MAGetGameInstance()))
	{
		FStompSubscriptionEvent chatEvent;

		TWeakObjectPtr<const UChatHandler> thisPtr = this;
		chatEvent.BindLambda([InChatRoomId, thisPtr](const IStompMessage& StompMessage)
		{
			if(thisPtr.IsValid())
			{
				FChatData chatData;
				TSharedPtr<FJsonObject> chatDataStr = UtilJson::StringToJson(StompMessage.GetBodyAsString());
				thisPtr->_JsonToData(chatDataStr, chatData);
				
				// 파싱해서 chatroomid와 data를 broadcast
				if(thisPtr->OnChatDelegate.IsBound())
					thisPtr->OnChatDelegate.Broadcast(InChatRoomId, chatData);

				// 내가 보냈던 채팅이 아니라면 Noti를 보내지 않음
				if(chatData.Sender == MAGetMyUserName(MAGetGameInstance()))
					return;
				
				const FNotificationManager* notiManager = MAGetNotificationManager(MAGetGameInstance());
				if(notiManager == nullptr)
					return;

				// Noti를 보낸다.
				notiManager->PushNotification(chatData.Sender, chatData.Content);
			}
		});
		
		FStompSubscriptionId subId = stompHandler->Subscribe(DA_NETWORK(WSReceiveChatAddURL) + FString::Printf(TEXT("/%d"), InChatRoomId), chatEvent);

		Sub1On1ChatRooms.Emplace(InChatRoomId, subId);
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
	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	if (InJsonObj->TryGetArrayField(TEXT("messageTime"), out) && (out->Num() >= 6)) // 년 월 일 시간 분 초를 모두 파싱할 수 있을지
	{
		OutItemReply.Time = FDateTime((*out)[0]->AsNumber(),
		                              (*out)[1]->AsNumber(),
										 (*out)[2]->AsNumber(),
										 (*out)[3]->AsNumber(),
										 (*out)[4]->AsNumber(),
										 (*out)[5]->AsNumber());
		
		return;
	}

	// 배열 형태가 아니었다면 string 형태로 파싱 시도
	FString timeString;
	if(InJsonObj->TryGetStringField(TEXT("messageTime"), timeString))
	{
		 FDateTime::ParseIso8601(*timeString, OutItemReply.Time);
	}
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
	InJsonObj->TryGetStringField(TEXT("title"), OutChatRoomData.Title);
	InJsonObj->TryGetNumberField(TEXT("currentPrice"), OutChatRoomData.Price);
}
