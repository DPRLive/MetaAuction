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

	FUserShareData() : UserName(TEXT("")), SelectedCharacter(-1) { }

	// 사용자 아이디 (이름)
	UPROPERTY()
	FString UserName;

	// 선택한 캐릭터
	UPROPERTY()
	int16 SelectedCharacter;
};