#include "NotificationManager.h"

FNotificationManager::FNotificationManager()
{
}

/**
 * 알림이 오면 뿌립니다.
 */
void FNotificationManager::PushNotification(const FString& InTitle, const FString& InContent) const
{
	if(OnNotificationDelegate.IsBound())
		OnNotificationDelegate.Broadcast(InTitle, InContent, FDateTime::Now());
}