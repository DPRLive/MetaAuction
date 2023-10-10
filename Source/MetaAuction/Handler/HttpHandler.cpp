// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpHandler.h"
#include "Util/UtilEnum.h"
#include "../Core/MAGameInstance.h"
#include "../DataAsset/NetworkDataAsset.h"

#include <HttpModule.h>

FHttpHandler::FHttpHandler()
{
	HttpModule = nullptr;
	HttpModule = &FHttpModule::Get();
}

FHttpHandler::~FHttpHandler()
{
	HttpModule = nullptr;
}

/**
 * Http 요청 함수
 * @param InAddUrl : HTTP 요청을 보낼 추가적인 Url (ex) /login/yes
 * @param InType : HTTP 요청을 보낼 타입
 * @param InFunc : HTTP 요청에 대한 응답이 왔을때 처리할 함수, 멤버함수면 람다로 멤버함수를 weak ptr에서 호출하는, 감싸는 형식 넘겨주면 됨. parameter는 UHTTPHandler.h의 using 보고 맞춰야함
 * @param InBody : Post 요청시 Body가 필요할때 Json 형식의 String 첨부
 */
void FHttpHandler::Request(const FString& InAddUrl, const EHttpRequestType InType, const FCompleteCallback& InFunc, const FString& InBody) const
{
	TSharedPtr<IHttpRequest> httpRequest = HttpModule->CreateRequest();
	httpRequest->SetVerb(ENUM_TO_STRING(EHttpRequestType, InType));
	httpRequest->SetURL(DA_NETWORK(ServerURL) + InAddUrl);
	
	// 로그인이 된 상태라면, 헤더에 JWT 토큰 정보를 담는다.
	if(UMAGameInstance* gameInstance = Cast<UMAGameInstance>(MAGetGameInstance()))
	{
		if(gameInstance->GetLoginData().bLogin)
		{
			httpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + gameInstance->GetLoginData().JwtToken);
		}
	}

	// POST 요청일 경우 Body를 담는다.
	if(InType == EHttpRequestType::POST)
	{
		httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		httpRequest->SetContentAsString(InBody);
	}
	
	httpRequest->OnProcessRequestComplete().BindLambda(InFunc);
	httpRequest->ProcessRequest();
}