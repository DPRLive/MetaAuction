#pragma once

#include <Serialization/JsonSerializer.h>

/**
 * Json 관련 Util 함수
 */
namespace UtilJson
{
	/**
	 * FJsonObject를, string으로 변환해주는 함수
	 * @param InObj : string으로 변환할 JSonObject
	 */ 
	FORCEINLINE FString JsonToString(const TSharedRef<FJsonObject>& InObj)
	{
		FString stringBody;
		TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&stringBody);
		FJsonSerializer::Serialize(InObj, writer);

		return stringBody;
	}
};
