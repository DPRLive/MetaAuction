#pragma once

// 로그 매크로 모음

/**
 * 출력 로그 (log) 용 로그
 */
#define LOG_N( text, ... )													\
{																			\
	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
	+ FString::Printf(text, ##__VA_ARGS__);									\
	UE_LOG(LogTemp, Log, TEXT("%s"), *msg);									\
}

/**
 * 출력 로그 (Warning Category) 용 로그
 */
#define LOG_WARN( text, ... )												\
{																			\
	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
	+ FString::Printf(text, ##__VA_ARGS__);									\
	UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);								\
}

/**
 * 출력 로그 (Error Category) 용 로그
 */
#define LOG_ERROR( text, ... )												\
{																			\
	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
	+ FString::Printf(text, ##__VA_ARGS__);									\
	UE_LOG(LogTemp, Error, TEXT("%s"), *msg);								\
}

/**
 * 출력로그 + 에디터 스크린 동시에 로그
 */
#define LOG_SCREEN( Color, text, ... )										\
{																			\
    LOG_WARN( text, ##__VA_ARGS__);											\
	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
	+ FString::Printf(text, ##__VA_ARGS__);									\
	GEngine->AddOnScreenDebugMessage( -1, 2.0f, Color, msg );				\
}

/**
 * 현재 Role을 확인하기 위한 매크로 (GetLocalRole(), GetRemoteRole()등)
 */
#define LOG_ROLE( GetRole )					\
FString MyRole;								\
switch (GetRole)							\
{											\
case ROLE_None:								\
	MyRole =  TEXT("ROLE_None");			\
	break;									\
case ROLE_SimulatedProxy:					\
	MyRole = TEXT("ROLE_SimulatedProxy");	\
	break;									\
case ROLE_AutonomousProxy:					\
	MyRole = TEXT("ROLE_AutonomousProxy");	\
	break;									\
case ROLE_Authority:						\
	MyRole = TEXT("ROLE_Authority");		\
	break;									\
default:									\
	MyRole = TEXT("Error");					\
	break;									\
}											\
LOG_WARN(TEXT("Hello. %s"), *MyRole);