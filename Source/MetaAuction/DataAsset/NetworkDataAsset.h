// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "NetworkDataAsset.generated.h"

/**
 * HTTP, STOMP WebSocket 통신시 사용할 데이터들
 */
UCLASS()
class METAAUCTION_API UNetworkDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 기본 WebServerURL
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString ServerURL = TEXT("localhost:8080");

	// 로그인 요청을 보낼 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString LoginAddURL = TEXT("/auth/login");

	// 모든 아이템 정보를 조회하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString AllItemAddURL = TEXT("/item/all");
	
	// 상품 아이디로 정보를 조회하는 Url, 뒤에 /1 이런식으로 id를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString ItemInfoAddURL = TEXT("/item");

	// 상품 검색용 추가 Url, body에 searchString, world, type, isPossible 등을 담을 수 있음
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString ItemSearchAddURL = TEXT("/item/search");

	// 상품 아이디로 glb 파일을 요청하는 Url, 뒤에 /{itemID} 붙여서 사용.
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString GlbFileDownAddURL = TEXT("/item/glb");
	
	// 상품 아이디와 ImgIdx로 이미지 파일을 요청하는 Url, 뒤에 /{itemID}/{fileNum}을 붙여서 사용.
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString ImgViewAddURL = TEXT("/item/image");

	// 입찰을 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString BidAddURL = TEXT("/bid");

	// 해당 물품의 입찰기록을 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString BidRecordAddURL = TEXT("/bid/record");

	// 해당 물품의 삭제를 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString RemoveItemAddURL = TEXT("/item/remove");
	
	// 내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString MySellItemAddURL = TEXT("/seller/myproduct");

	// 내가 구매 성공한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString MyBuyItemAddURL = TEXT("/buyer/myproduct");

	// 내가 구매(입찰) 시도한 물품을 요청하는 추가 Url
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString MyBidItemAddURL = TEXT("/buyer/tryrecord");

	// 물품의 최근 수정 시간을 가져오는 추가 Url, /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString ModifyTimeAddURL = TEXT("/item/lastmodifiedtime");
	
	// STOMP WebSocket 기본 연결 URL
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSServerURL = TEXT("ws://localhost:8080/ws");
	
	// STOMP WebSocket 새 상품 알림 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSNewItemAddURL = TEXT("/sub/newItem");

	// STOMP WebSocket 물품 삭제 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSRemoveItemAddURL = TEXT("/sub/endItem");

	// STOMP WebSocket 물품 가격 변동 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSChangePriceAddURL = TEXT("/sub/changePrice");

	// STOMP WebSocket 물품 정보 변동 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSChangeDataAddURL = TEXT("/sub/modify");

	// STOMP WebSocket 채팅&댓글 보내기 추가 url, /{itemid}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSSendChatAddURL = TEXT("/pub/chat");

	// STOMP WebSocket 채팅&댓글 받기 추가 url
	// 물품 댓글 받기의 경우 : /{itemid}를 붙여서 사용
	// 1대1 채팅 받기의 경우 : /{itemId}-{sellerUsername}-{buyerUsername}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSReceiveChatAddURL = TEXT("/sub/chat");
	
	// 웹서버 시간을 UTC에 맞추기 위해 변화해야 하는 변화 값 설정
	UPROPERTY( EditDefaultsOnly, Category = "Server" )
	FTimespan WebServerUTCDiff = - FTimespan(9, 0, 0);
};