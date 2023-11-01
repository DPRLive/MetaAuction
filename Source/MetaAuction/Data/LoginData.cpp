#include "LoginData.h"

#include <Misc/Base64.h>
#include <Serialization/JsonReader.h>
#include <Serialization/JsonSerializer.h>

FLoginData::FLoginData(const FString& InJwtToken)
{
	// Jwt 토큰을 저장 후 파싱한다
	JwtToken = InJwtToken;
	UserName = _ParseUserNameInToken(JwtToken);

	LOG_N(TEXT("Welcome [%s]! Login Success!"), *UserName);
}

/**
 * Jwt Token을 파싱하여 username을 뽑아낸다.
 */
FString FLoginData::_ParseUserNameInToken(const FString& InJwtToken) const
{
	// jwt token에는 '.' 이 두개 있다. 그걸 기준으로 3등분
	TArray<FString> parseArr;
	InJwtToken.ParseIntoArray(parseArr, TEXT("."), true);

	if(parseArr.Num() < 3)
	{
		LOG_ERROR(TEXT("JwtToken is Unvalid!"));
		return TEXT("");
	}

	// user name을 담고 있는 중간 부분을 디코딩
	FString decodeStr;
	if(!FBase64::Decode(parseArr[1], decodeStr))
	{
		LOG_ERROR(TEXT("Parse Username in JwtToken is Failed!"));
		return TEXT("");
	}

	// json object로 변환하여 username을 뽑아낸다
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(decodeStr);
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(reader, jsonObject);

	FString userName;
	jsonObject->TryGetStringField(TEXT("username"), userName);
	
	return userName;
}
