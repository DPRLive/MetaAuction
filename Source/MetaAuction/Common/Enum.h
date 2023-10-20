﻿#pragma once

#include <Misc/EnumRange.h>

/**
 * Enum 모음
 */

/**
 * HTTP 통신 요청 타입을 정의
 */
UENUM()
enum class EHttpRequestType : uint8
{
	GET,
	POST,
	DELETE
};

// 상품의 판매 타입(경매인가 판매인가)
UENUM(BlueprintType)
enum class EItemDealType : uint8
{
	None		UMETA( Tooltip = "판매 타입 지정 안됨" ),
	Auction		UMETA( Tooltip = "경매 타입" ),
	Normal		UMETA( Tooltip = "일반 판매 타입" )
};

// 상품이 현재 판매중인지
UENUM(BlueprintType)
enum class EItemCanDeal : uint8
{
	None		UMETA( Tooltip = "판매중인지 지정 안됨" ),
	Possible	UMETA( Tooltip = "판매중" ),
	Impossible	UMETA( Tooltip = "판매중 아님" )
};

// 내 아이템 검색 시 어떤 아이템을 원하는지
UENUM(BlueprintType)
enum class EMyItemReqType : uint8
{
	Sell		UMETA( Tooltip = "내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품" ),
	Buy			UMETA( Tooltip = "내가 구매 성공한 물품" )
};