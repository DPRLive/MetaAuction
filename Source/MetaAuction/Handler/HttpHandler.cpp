// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpHandler.h"
#include "Util/UtilEnum.h"
#include "../Core/MAGameInstance.h"
#include "../DataAsset/NetworkDataAsset.h"

#include <HttpModule.h>
#include <Serialization/JsonSerializer.h>
#include <Serialization/JsonWriter.h>

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

/**
 * HTTP Body를 혹시 FJsonObject로 만들었다면, string으로 변환해주는 함수
 * @param InObj : string으로 변환할 JSonObject 
 */
FString FHttpHandler::JsonToString(const TSharedRef<FJsonObject>& InObj) const
{
	FString stringBody;
	TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&stringBody);
	FJsonSerializer::Serialize(InObj, writer);

	return stringBody;
}

