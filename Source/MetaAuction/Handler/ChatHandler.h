// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/NoExportTypes.h>
#include "ChatHandler.generated.h"

/**
 * 아이템 댓글 & 채팅에 관한 데이터 입니다.
 */
USTRUCT()
struct FChatData
{
	GENERATED_BODY()

	FChatData() : Sender(TEXT("")), Content(TEXT("")), Time(FDateTime()) {}

	// 채팅 or 댓글 쓴 사람
	FString Sender;

	// 내용
	FString Content;

	// 채팅 or 댓글 단 시간 (서울 시간)
	FDateTime Time;
	
};

/**
 * 한 채팅방에 관한 데이터 입니다.
 */
USTRUCT()
struct FChatRoomData
{
	GENERATED_BODY()

	FChatRoomData() : ChatRoomId(0), ItemId(0), Title(TEXT("알 수 없음")), Price(0), Buyer(TEXT("")), Seller(TEXT("")) {}

	// 해당 채팅방에 대한 고유 id, 0이면 invalid
	uint32 ChatRoomId;
	
	// 어떤 상품에 대한 채팅방인지
	uint32 ItemId;
	
	// 상품 제목, 판매자가 상품을 삭제했다면 "알 수 없음" 입니다.
	FString Title;

	// 상품의 최종 가격
	uint64 Price;

	// 판매자
	FString Buyer;
	
	// 구매자
	FString Seller;
};

/**
 * 채팅 내역 요청시 사용하는 enum 입니다.
 */
UENUM()
enum class ERequestChatType : uint8
{
	Chatroom		UMETA(Tooltip = "1대1 채팅방"),
	ItemReply		UMETA(Tooltip = "물품 댓글")
};

/**
 * 채팅과 관련된 Handler class입니다.
 * 물품에 대한 댓글도 채팅으로 간주하여, (웹 소켓씀) 해당 클래스에서 관리합니다.
 * ChatHandler는 특성상, 로그인 된 상태에서만 사용해야 합니다.
 */
UCLASS()
class METAAUCTION_API UChatHandler : public UObject
{
	GENERATED_BODY()

public:	
	UChatHandler();

	// ChatHandler를 초기화 합니다.
	void InitChatHandler();

private:
	// 로그인 이후 로직을 처리합니다.
	void _AfterLogin(bool InbSuccess);

public:
	// Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
	void AddReplyToItem(const uint32 InItemId, const FString& InContent) const;

	// Stomp로 InChatRoomDta에 맞는 채팅방에 1대1 거래 채팅을 보낸다. 로그인 된 경우만 사용 가능
	void Send1On1Chat(const FChatRoomData& InChatRoomData, const FString& InContent) const;
	
	// Stomp로 해당 item ID에 달리는 댓글을 수신합니다. 여러함수를 바인드 할 수 있습니다.
	// 댓글 수신이 필요 없어지면 (상세 정보 창을 닫는다던가)
	// 꼭 리턴받은 FDelegateHandle로 해당 함수를 UnsubscribeItemReply 해주세요! 메모리 누수가 발생할 수 있습니다.
	FDelegateHandle SubscribeItemReply(const uint32 InItemId, const FCallbackRefOneParam<FChatData>& InFunc);
	
	// Stomp로 해당 item ID에 달리는 댓글을 수신하던 함수를 delegate에서 해제합니다.
	void UnsubscribeItemReply(const uint32 InItemId, const FDelegateHandle& InHandle);

	// id로 채팅 or 댓글들을 요청합니다.
	// InChatType이 Chatroom일 경우 : id는 채팅방 id로 사용합니다.
	// InChatType이 ItemReply일 경우 : id는 물품 id로 사용합니다.
	void RequestChatsById(const ERequestChatType InChatType, const uint32 InId, const FCallbackRefArray<FChatData>& InFunc) const;

	// 나의 채팅방 목록을 모두 불러옵니다. (로그인 된 상태에서만 사용 가능)
	void RequestMyChatRoom(const FCallbackRefArray<TPair<FChatRoomData, FChatData>>& InFunc);

	// 새로운 채팅방을 생성합니다. 경매의 경우는 끝나면 자동으로 채팅방이 생성되니 일반 판매일 때만 사용하면 됩니다.
	// (특정 유저가 어떤 일반 판매 물품을 사고싶으면 이 api를 호출해서 채팅을 시작하면 됨)
	void RequestNewChatRoom(const uint32 InItemId, const FString& InSellerName, const FCallbackRefOneParam<FChatRoomData>& InFunc);
private:
	// 웹소켓에 1대1 채팅을 Subscribe하여 특정 채팅방에 오는 채팅을 모두 받는다.
	void _TrySubscribe1On1Chat(const uint32 InChatRoomId);
	
	// Json 형태의 ItemReply JsonObject를 FItemReply로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatData& OutItemReply) const;

	// Json 형태의 ChatRoomData JsonObject를 FChatRoomData 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatRoomData& OutChatRoomData) const;
	
	// 댓글 수신 시 뿌려주는 역할을 담당할 Delegate
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemReplyDelegate, const FChatData&);

	// 아이템 댓글이 달릴때 호출할 Delegate와 Stomp의 어떤 event에 bind 되었는지, item id로 접근
	using FReplyDelegateInfo = TPair<FStompSubscriptionId, TSharedPtr<FOnItemReplyDelegate>>;
	TMap<uint32, FReplyDelegateInfo> ItemReplyDelegates;

	// 현재 구독해서 메세지를 받고있는 채팅방들을 관리합니다.
	TMap<uint32, FStompSubscriptionId> Sub1On1ChatRooms;
public:
	// 1대1 채팅이 오면, (어느 채팅방에서 왔든) 이곳으로 전달 받으면 됩니다.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChatDelegate, const uint32 /* InChatroomId */, const FChatData& /* InChatData */);
	FOnChatDelegate OnChatDelegate;
};
