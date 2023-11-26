// Fill out your copyright notice in the Description page of Project Settings.


#include "MALobbyGameMode.h"
#include "UI/Title/MALoginWidget.h"

#include <Blueprint/UserWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MALobbyGameMode)

AMALobbyGameMode::AMALobbyGameMode()
{
	LoginWidgetPtr = nullptr;
}

/**
 *  시작시 호출
 */
void AMALobbyGameMode::StartPlay()
{
	Super::StartPlay();

	//	로그인 UI를 띄웁니다.
	if(APlayerController* localController = GetWorld()->GetFirstPlayerController())
	{
		LoginWidgetPtr = CreateWidget<UMALoginWidget>(localController, LoginWidgetClass);

		if(IsValid(LoginWidgetPtr))
			LoginWidgetPtr->AddToViewport();
	}
}
