#pragma once

// 로그 매크로 모음

/**
 * 출력 로그 (log) 용 로그
 */
#define LOG_N( text, ... )													\
{																			\
	FString msg = FString::Printf(text, ##__VA_ARGS__);						\
	UE_LOG(LogTemp, Log, TEXT("%s"), *msg);									\
}

/**
 * 출력 로그 (Warning Category) 용 로그
 */
#define LOG_WARN( text, ... )												\
{																			\
	FString msg = FString::Printf(text, ##__VA_ARGS__);						\
	UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);								\
}

/**
 * 출력 로그 (Error Category) 용 로그
 */
#define LOG_ERROR( text, ... )												\
{																			\
	FString msg = FString::Printf(text, ##__VA_ARGS__);						\
	UE_LOG(LogTemp, Error, TEXT("%s"), *msg);								\
}

/**
 * 출력로그 + 에디터 스크린 동시에 로그
 */
#define LOG_SCREEN( Color, text, ... )										\
{																			\
    LOG_WARN( text, ##__VA_ARGS__);											\
	FString msg = FString::Printf(text, ##__VA_ARGS__);						\
	GEngine->AddOnScreenDebugMessage( -1, 2.0f, Color, msg );				\
}