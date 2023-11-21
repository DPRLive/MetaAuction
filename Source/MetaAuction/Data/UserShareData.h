#pragma once

#include "UserShareData.generated.h"
/**
 * 다른 유저와도 공유해야할 Share Data입니다.
 * PlayerState를 통해 다른 유저에게 전파됩니다.
 */
USTRUCT()
struct FUserShareData
{
	GENERATED_BODY()

	FUserShareData() : UserName(TEXT("")) { }

	FUserShareData& operator=(const FUserShareData& InUserData)
	{
		UserName = InUserData.UserName;
		return *this;
	}

	FUserShareData& operator=(FUserShareData&& InUserData) noexcept
	{
		UserName = MoveTemp(InUserData.UserName);
		return *this;
	}
	
	UPROPERTY()
	FString UserName;
};