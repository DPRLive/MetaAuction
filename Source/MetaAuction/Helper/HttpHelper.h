// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FJsonObject;
class FHttpModule;

/**
 * Spring Server와 HTTP 통신을 위한 Helper
 */
class FHttpHelper
{
public:
	// Request가 Complete되었을때 함수 타입 정의
	using FCompleteCallback = TFunction<void(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)>;
	
	FHttpHelper();

	~FHttpHelper();
	
	// Http 요청 함수
	void Request(const FString& InAddUrl, const EHttpRequestType InType, const FCompleteCallback& InFunc, const FString& InBody = TEXT("")) const;

	// HTTP Body를 혹시 FJsonObject로 만들었다면, string으로 변환해주는 함수
	FString JsonToString(const TSharedRef<FJsonObject>& InObj) const;
private:
	// Http 요청에 사용할 모듈
	FHttpModule* HttpModule;
};