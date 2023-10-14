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
	
	// 상품 아이디로 파일을 요청하는 Url, 뒤에 /{itemID}/{fileNum}을 붙여서 사용. 1 : glb, 2~ 사진 파일
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString FileDownAddURL = TEXT("/item/download");

	// 입찰을 요청하는 추가 Url, 뒤에 /{itemID}를 붙여서 사용
	UPROPERTY( EditDefaultsOnly, Category = "HTTP" )
	FString BidAddURL = TEXT("/bid");
	
	// STOMP WebSocket 기본 연결 URL
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString WSServerURL = TEXT("ws://localhost:8080/ws");
	
	// STOMP WebSocket 새 상품 알림 구독용 추가 url
	UPROPERTY( EditDefaultsOnly, Category = "WebSocket" )
	FString NewItemAddURL = TEXT("/sub/newItem");
};
