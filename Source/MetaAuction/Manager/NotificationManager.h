#pragma once

/**
 * 알림을 처리하는 매니저입니다.
 */
class FNotificationManager
{
public:
	FNotificationManager();

	// 알림을 넣습니다.
	void PushNotification(const FString& InTitle, const FString& InContent) const;

	// 알림을 여기서 받아가면 됩니다.
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnNotificationDelegate, const FString& /* Title */, const FString& /* Content */, const FDateTime& /* Time */)
	FOnNotificationDelegate OnNotificationDelegate;
};
