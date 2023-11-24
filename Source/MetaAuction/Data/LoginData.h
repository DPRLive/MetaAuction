#pragma once

/**
 * Login Data를 담는 클래스
 */
class FLoginData
{
public:
	FLoginData() = delete;

	FLoginData(const FString& InJwtToken);

	// JWT Token Getter
	FORCEINLINE const FString& GetJwtToken() const { return JwtToken; }

	// UserName Getter, Jwt Token을 파싱하여 username을 뽑아낸다.
	FString GetMyUserName() const;
	
private:
	// 저장할 JwtToken
	FString JwtToken;
};
