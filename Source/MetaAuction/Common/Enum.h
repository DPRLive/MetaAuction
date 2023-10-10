#pragma once

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
	POST
};

// 테스트용
UENUM(BlueprintType)
enum class MyEnum
{
	None,
	First,
	Max
};
ENUM_RANGE_BY_COUNT(MyEnum, MyEnum::Max)