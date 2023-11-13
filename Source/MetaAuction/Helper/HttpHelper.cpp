// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpHelper.h"
#include "Data/LoginData.h"
#include "../Core/MAGameInstance.h"
#include "../DataAsset/NetworkDataAsset.h"

#include <HttpModule.h>

FHttpHelper::FHttpHelper()
{
	HttpModule = nullptr;
	HttpModule = &FHttpModule::Get();
}

FHttpHelper::~FHttpHelper()
{
	HttpModule = nullptr;
}

/**
 * Http 요청 함수
 * @param InAddUrl : HTTP 요청을 보낼 추가적인 Url (ex) /login/yes
 * @param InType : HTTP 요청을 보낼 타입
 * @param InFunc : HTTP 요청에 대한 응답이 왔을때 처리할 함수, 멤버함수면 람다로 멤버함수를 weak ptr에서 호출하는, 감싸는 형식 넘겨주면 됨. parameter는 UHTTPHandler.h의 using 보고 맞춰야함
 * @param InBody : Post 요청시 Body가 필요할때 Json 형식의 String 첨부
 * @param InJwtToken : Jwt 토큰을 따로 설정하고 싶다면 설정, 아닌경우 나의 token을 기본으로 사용함
 */
void FHttpHelper::Request(const FString& InAddUrl, const EHttpRequestType InType, const FCompleteCallback& InFunc, const FString& InBody, const FString& InJwtToken) const
{
	TSharedPtr<IHttpRequest> httpRequest = HttpModule->CreateRequest();
	httpRequest->SetVerb(ENUM_TO_STRING(EHttpRequestType, InType));
	httpRequest->SetURL(DA_NETWORK(ServerURL) + InAddUrl);

	// 토큰이 따로 설정된 경우 그거 설정
	if(InJwtToken != TEXT(""))
	{
		httpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + InJwtToken);
	}
	else if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
	{
		// 로그인이 된 상태라면, 헤더에 JWT 토큰 정보를 담는다.
		const TWeakPtr<FLoginData> loginData = gameInstance->GetLoginData();
		if(loginData.IsValid())
		{
			httpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + loginData.Pin()->GetJwtToken());
		}
	}
	
	// Body가 있을 경우 담는다.
	// 왠지 모르겠는데 unreal에서 get요청에 바디 담으니까 요청을 못보냄
	if(InType == EHttpRequestType::POST)
	{
		httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		httpRequest->SetContentAsString(InBody);
	}

	httpRequest->OnProcessRequestComplete().BindLambda(InFunc);
	httpRequest->ProcessRequest();
}

