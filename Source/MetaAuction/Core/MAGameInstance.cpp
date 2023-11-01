// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MAGameInstance.h"
#include "Data/LoginData.h"

#include <Serialization/JsonSerializer.h>
#include <Interfaces/IHttpResponse.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameInstance)

UMAGameInstance::UMAGameInstance()
{
}

/**
 * 게임이 시작될 때 호출됨 ( 에디터의 경우 시작 버튼을 눌렀을 때 )
 */
void UMAGameInstance::Init()
{
	Super::Init();

	HttpHelper = MakeShareable(new FHttpHelper());
	StompHelper = MakeShareable(new FStompHelper());
	
	//if(IsRunningDedicatedServer()) // 테스트, 데디 서버면 자동 로그인
	//{
		LOG_WARN(TEXT("Is Running Dedicated Server! Auto Login!"));
		RequestLogin(TEXT("test"), TEXT("test"));
	//}
	
	if(!IsRunningDedicatedServer())
	{
		ItemFileHandler = MakeShareable(new FItemFileHandler());
	}
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
 * TODO : 추후 UI와 연동되며 로그인이 완료 되었는지 알아야하면 Delegate 추가할 예정
 * @param InID : 유저 ID
 * @param InPassword : 유저 패스워드
 */
void UMAGameInstance::RequestLogin(const FString& InID, const FString& InPassword)
{
	if(LoginData.IsValid())
	{
		LOG_N(TEXT("이미 로그인 되어있음"));
		return;
	}

	// Body를 만든다.
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	requestObj->SetStringField(TEXT("username"), InID);
	requestObj->SetStringField(TEXT("password"), InPassword);
	
	if(HttpHelper.IsValid())
	{
		// 로그인을 요청한다.
		TWeakObjectPtr<UMAGameInstance> thisPtr = this;
		HttpHelper->Request(DA_NETWORK(LoginAddURL), EHttpRequestType::POST, [thisPtr](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		{
			if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
			{
				// 로그인 요청 성공
				// Json reader 생성
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InResponse->GetContentAsString());
				TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
				FJsonSerializer::Deserialize(reader, jsonObject);
				
				// 로그인 데이터를 저장한다.
				FString jwtToken = jsonObject->GetStringField(TEXT("accessToken"));
				thisPtr->LoginData = MakeShareable(new FLoginData(jwtToken));
			}
			else
			{
				LOG_ERROR(TEXT("로그인 실패 !"))
			}
		}, HttpHelper->JsonToString(requestObj));
	}
}
