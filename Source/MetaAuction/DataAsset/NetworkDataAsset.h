// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "NetworkDataAsset.generated.h"

/**
 * HTTP, STOMP WebSocket 통신시 사용할 데이터들
 */
UCLASS(config = MetaAuction)
class METAAUCTION_API UNetworkDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 로그인 요청을 보낼 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString LoginAddURL = TEXT("/auth/login");

	// 모든 아이템 정보를 조회하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString AllItemAddURL = TEXT("/item/all");
	
	// 상품 아이디로 정보를 조회하는 Url, 뒤에 /1 이런식으로 id를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString ItemInfoAddURL = TEXT("/item");

	// 상품 검색용 추가 Url, body에 searchString, world, type, isPossible 등을 담을 수 있음
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString ItemSearchAddURL = TEXT("/item/search");

	// 상품 아이디로 glb 파일을 요청하는 Url, 뒤에 /{itemID} 붙여서 사용.
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|File" )
	FString GlbFileDownAddURL = TEXT("/item/glb");
	
	// 상품 아이디와 ImgIdx로 이미지 파일을 요청하는 Url, 뒤에 /{itemID}/{fileNum}을 붙여서 사용.
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|File" )
	FString ImgViewAddURL = TEXT("/item/image");

	// 입찰을 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Bid" )
	FString BidAddURL = TEXT("/bid");

	// 해당 물품의 입찰기록을 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Bid" )
	FString BidRecordAddURL = TEXT("/bid/record");

	// 해당 물품의 삭제를 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString RemoveItemAddURL = TEXT("/item/remove");
	
	// 내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString MySellItemAddURL = TEXT("/seller/myproduct");

	// 내가 구매 성공한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString MyBuyItemAddURL = TEXT("/buyer/myproduct");

	// 내가 구매(입찰) 시도한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Bid" )
	FString MyBidItemAddURL = TEXT("/buyer/tryrecord");

	// 물품의 최근 수정 시간을 가져오는 추가 Url, /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString ModifyTimeAddURL = TEXT("/item/lastmodifiedtime");

	// 물품의 댓글을 가져오는 추가 Url, /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Chat" )
	FString ItemReplyAddURL = TEXT("/item/chat");

	// 채팅방의 채팅을 가져오는 추가 Url, /{chatRoomId}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Chat" )
	FString ChatroomChatAddURL = TEXT("/chat");
	
	// 나의 채팅방들을 가져오는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Chat" )
	FString MyChatRoomAddURL = TEXT("/mychatroomAll");

	// 새로운 채팅방을 만드는 추가 Url, /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Chat" )
	FString CreateChatRoomAddURL = TEXT("/create/chatRoom");

	// 물품의 판매자를 검증하는 추가 Url, /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP|Item" )
	FString ValidateItemAddURL = TEXT("/item/isitmine");
	
	// STOMP WebSocket 새 상품 알림 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Item" )
	FString WSNewItemAddURL = TEXT("/sub/newItem");

	// STOMP WebSocket 물품 삭제 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Item" )
	FString WSRemoveItemAddURL = TEXT("/sub/endItem");

	// STOMP WebSocket 물품 가격 변동 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Item" )
	FString WSChangePriceAddURL = TEXT("/sub/changePrice");

	// STOMP WebSocket 물품 정보 변동 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Item" )
	FString WSChangeDataAddURL = TEXT("/sub/modify");

	// STOMP WebSocket 채팅&댓글 보내기 추가 url, /{itemid}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Chat" )
	FString WSSendChatAddURL = TEXT("/pub/chat");

	// STOMP WebSocket 채팅 받기 추가 url /{chatroomid}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Chat" )
	FString WSReceiveChatAddURL = TEXT("/sub/privateChat");

	// STOMP WebSocket 1대1 채팅 받기 추가 url /{chatroomid}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Chat" )
	FString WSReceiveReplyAddURL = TEXT("/sub/chat");

	// STOMP WebSocket 새로운 채팅방이 생길 시 오는 알림을 구독, %s에 username을 넣어서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket|Chat" )
	FString WSReceiveNewChatAddURL = TEXT("/sub/%s/newChat");
	
	// 웹서버 시간을 UTC에 맞추기 위해 변화해야 하는 변화 값 설정
	UPROPERTY( EditDefaultsOnly, Category = "Server" )
	FTimespan WebServerUTCDiff = - FTimespan(9, 0, 0);

	// 현재 데디 서버가 어떤 월드를 담당할지 string입니다.
	UPROPERTY( EditDefaultsOnly, Category = "World|Server" )
	FString DediWorld = TEXT("1");

	// 레벨 이동 시 사용할 로비 레벨 대한 정보입니다.
	UPROPERTY( EditDefaultsOnly, Category = "World" )
	FString LobbyUrl = TEXT("/Game/MetaAuction/Map/LobbyMap");

	// 기본 WebServerURL. (config 파일 사용)
	UPROPERTY( Config )
	FString ServerURL = TEXT("");
	
	// STOMP WebSocket 기본 연결 URL. (config 파일 사용)
	UPROPERTY( Config )
	FString WSServerURL = TEXT("");
	
	// 레벨 이동 시 사용할 데디 서버 레벨 대한 정보입니다. (config 파일 사용)
	UPROPERTY( Config )
	FString AuctionUrl = TEXT("");
	
	// 웹페이지의 회원가입 하는 url입니다. (config 파일 사용)
	UPROPERTY( Config )
	FString SignUpUrl = TEXT("");
};