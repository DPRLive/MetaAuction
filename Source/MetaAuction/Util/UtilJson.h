#pragma once

#include <Serialization/JsonSerializer.h>

/**
 * Json 관련 Util 함수
 */
namespace UtilJson
{
	/**
	 * Json 형태의 string을 JsonObject 로 변환해주는 함수
	 * @param InString : JsonObject 변환할 String
	 */
	FORCEINLINE TSharedPtr<FJsonObject> StringToJson(const FString& InString)
	{
		// Json reader 생성
		TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InString);
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		FJsonSerializer::Deserialize(reader, jsonObject);

		return jsonObject;
	}

	/**
	 * Json 형태를 담은 배열 형태의 string을 JsonObject Array 로 변환해주는 함수
	 * @param InString : string으로 변환할 Json Array 형태의 string
	 * @param OutArray : 담아갈 배열 
	 */
	FORCEINLINE void StringToJsonValueArray(const FString& InString, TArray<TSharedPtr<FJsonObject>>& OutArray)
	{
		// Json reader 생성
		TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(InString);
		TArray<TSharedPtr<FJsonValue>> jsonValues;
		FJsonSerializer::Deserialize(reader, jsonValues);

		for(const TSharedPtr<FJsonValue>& jsonValue : jsonValues)
		{
			OutArray.Emplace(jsonValue->AsObject());
		}
	}
	
	/**
	 * FJsonObject를, string으로 변환해주는 함수
	 * @param InObj : string으로 변환할 JsonObject
	 */ 
	FORCEINLINE FString JsonToString(const TSharedRef<FJsonObject>& InObj)
	{
		FString stringBody;
		TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&stringBody);
		FJsonSerializer::Serialize(InObj, writer);

		return stringBody;
	}
};
