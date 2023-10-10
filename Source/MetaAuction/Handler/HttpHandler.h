// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FHttpModule;

/**
 * Spring Server와 HTTP 통신을 위한 Handler
 */
class FHttpHandler
{
public:
	// Request가 Complete되었을때 함수 타입 정의
	using FCompleteCallback = TFunction<void(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)>;
	
	FHttpHandler();

	~FHttpHandler();
	
	// Http 요청 함수
	void Request(const FString& InAddUrl, const EHttpRequestType InType, const FCompleteCallback& InFunc, const FString& InBody = TEXT("")) const;
	
private:

	// Http 요청에 사용할 모듈
	FHttpModule* HttpModule;
};