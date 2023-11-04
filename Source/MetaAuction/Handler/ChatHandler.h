// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
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

	FChatRoomData() : ChatRoomId(0), ItemId(0), Buyer(TEXT("")), Seller(TEXT("")) {}

	// 해당 채팅방에 대한 고유 id, 0이면 invalid
	uint32 ChatRoomId;
	
	// 어떤 상품에 대한 채팅방인지
	uint32 ItemId;

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
 * ChatHandler는 특성상, 로그인 된 상태에서만 사용 가능합니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UChatHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChatHandler();

protected:
	virtual void BeginPlay() override;

public:
	// Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
	void AddReplyToItem(const uint32 InItemId, const FString& InContent) const;

	// Stomp로 해당 item ID에 달리는 댓글을 수신합니다. 여러함수를 바인드 할 수 있습니다.
	// 댓글 수신이 필요 없어지면 (상세 정보 창을 닫는다던가)
	// 꼭 리턴받은 FDelegateHandle로 해당 함수를 UnsubscribeItemReply 해주세요! 메모리 누수가 발생할 수 있습니다.
	// Lambda 작성 시, this를 캡처한다면 weak 캡처 후 is valid 체크 한번 해주세요!
	FDelegateHandle SubscribeItemReply(const uint32 InItemId, const FCallbackRefOneParam<FChatData>& InFunc);
	
	// Stomp로 해당 item ID에 달리는 댓글을 수신하던 함수를 delegate에서 해제합니다.
	void UnsubscribeItemReply(const uint32 InItemId, const FDelegateHandle& InHandle);

	// id로 채팅 or 댓글들을 요청합니다.
	// InChatType이 Chatroom일 경우 : id는 채팅방 id로 사용합니다.
	// InChatType이 ItemReply일 경우 : id는 물품 id로 사용합니다.
	void RequestChatsById(const ERequestChatType InChatType, const uint32 InId, const FCallbackRefArray<FChatData>& InFunc) const;

	// 나의 채팅방 목록을 모두 불러옵니다. (로그인 된 상태에서만 사용 가능)
	void RequestMyChatRoom(const FCallbackRefArray<FChatRoomData>& InFunc) const;
private:
	// Json 형태의 ItemReply JsonObject를 FItemReply로 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatData& OutItemReply) const;

	// Json 형태의 ChatRoomData JsonObject를 FChatRoomData 변환한다.
	void _JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FChatRoomData& OutChatRoomData) const;
	
	// 댓글 수신 시 뿌려주는 역할을 담당할 Delegate
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemReply, const FChatData&);

	// 아이템 댓글이 달릴때 호출할 Delegate와 Stomp의 어떤 event에 bind 되었는지, item id로 접근
	using FReplyDelegateInfo = TPair<FStompSubscriptionId, TSharedPtr<FOnItemReply>>;
	TMap<uint32, FReplyDelegateInfo> ItemReplyDelegates;
};
