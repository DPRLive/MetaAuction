#pragma once

/**
 * 채팅과 관련된 Handler class입니다.
 * 물품에 대한 댓글도 채팅으로 간주하여, (웹 소켓씀) 해당 클래스에서 관리합니다.
 * ChatHandler는 특성상, 로그인 된 상태에서만 사용 가능합니다.
 */
class FChatHandler
{
public:
	// Stomp로 item id에 맞는 상품에 댓글을 답니다. 로그인 된 경우만 사용 가능
	void AddReplyToItem(const uint32 InItemId, const FString& InContent) const;
};
