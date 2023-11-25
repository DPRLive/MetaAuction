// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/MAGameInstance.h"
#include "Data/LoginData.h"
#include "MAPlayerState.h"

#include <GameFramework/PlayerController.h>
#include <Serialization/JsonSerializer.h>
#include <Interfaces/IHttpResponse.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameInstance)

UMAGameInstance::UMAGameInstance()
{
	LoginData = nullptr;
	TempUserShareData = FUserShareData();
	ItemFileHandler = nullptr;
	ChatHandler = nullptr;
	HttpHelper = nullptr;
	StompHelper = nullptr;
}

/**
 * Init() 이후에 호출됨
 */
void UMAGameInstance::OnStart()
{
	Super::OnStart();
	
	if(!IsRunningDedicatedServer())
	{
		////// Only Client //////
		ItemFileHandler = MakeShareable(new FItemFileHandler());

		ChatHandler = NewObject<UChatHandler>(this);
		ChatHandler->InitChatHandler();

		OnNotifyPreClientTravel().AddUObject(this, &UMAGameInstance::_SaveMyUserShareData);

		// 클라 로그인 테스트 //
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]()
		{
		   LOG_WARN(TEXT("Client Login Test"));
		   RequestLogin(TEXT("test"), TEXT("test"));
		}, 5.f, false);
	}
}

/**
 * 게임이 시작될 때 호출됨 ( 에디터의 경우 시작 버튼을 눌렀을 때 )
 */
void UMAGameInstance::Init()
{
	Super::Init();

	HttpHelper = MakeShareable(new FHttpHelper());
	StompHelper = MakeShareable(new FStompHelper());
} 

/**
 * 게임이 종료될 때마다 호출됨 ( 에디터의 경우 종료 버튼을 눌렀을 때 )
 */
void UMAGameInstance::Shutdown()
{
	Super::Shutdown();
}

/**
 *	웹서버에 사용자 로그인을 요청하는 함수
 * @param InID : 유저 ID
 * @param InPassword : 유저 패스워드
 */
void UMAGameInstance::RequestLogin(const FString& InID, const FString& InPassword)
{
	// Body를 만든다.
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	requestObj->SetStringField(TEXT("username"), InID);
	requestObj->SetStringField(TEXT("password"), InPassword);
	
	if(!HttpHelper.IsValid())
		return;
	
	TWeakObjectPtr<UMAGameInstance> thisPtr = this;
	HttpHelper->Request(DA_NETWORK(LoginAddURL), EHttpRequestType::POST,
	                    [thisPtr](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
	                    {
		                    if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
		                    {
			                    // 로그인 요청 성공
			                    const TSharedPtr<FJsonObject> jsonObject = UtilJson::StringToJson(InResponse->GetContentAsString());

			                    // 로그인 데이터를 저장한다.
			                    FString jwtToken = jsonObject->GetStringField(TEXT("accessToken"));
			                    thisPtr->LoginData = MakeShareable(new FLoginData(jwtToken));

			                    // 로컬 유저의 Player State에 이름을 저장한다.
			                    if (APlayerController* controller = thisPtr->GetFirstLocalPlayerController())
			                    {
				                    if (AMAPlayerState* playerState = controller->GetPlayerState<AMAPlayerState>())
				                    {
					                    playerState->ServerRPC_SendUserName(thisPtr->LoginData->GetMyUserName());
				                    }
			                    }

			                    if (thisPtr->OnLoginDelegate.IsBound())
				                    thisPtr->OnLoginDelegate.Broadcast(true);
			                    return;
		                    }

		                    LOG_ERROR(TEXT("로그인 실패 !"));

		                    if (thisPtr->OnLoginDelegate.IsBound())
			                    thisPtr->OnLoginDelegate.Broadcast(false);
	                    }, UtilJson::JsonToString(requestObj));
	
}

/**
 * OnNotifyPreClientTravel에 등록되어, 레벨 이동이 일어나기 전 나의 UserData를 저장합니다. 
 */
void UMAGameInstance::_SaveMyUserShareData(const FString& String, ETravelType Travel, bool Cond)
{
	LOG_WARN(TEXT("Pre travel! save user data."));
	if (APlayerController* controller = GetFirstLocalPlayerController(GetWorld()))
	{
		if (AMAPlayerState* playerState = controller->GetPlayerState<AMAPlayerState>())
		{
			TempUserShareData = playerState->GetUserData();
		}
	}
}
