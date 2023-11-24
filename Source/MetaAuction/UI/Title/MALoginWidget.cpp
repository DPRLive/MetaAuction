// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MALoginWidget.h"
#include "Core/MAGameInstance.h"

#include <Components/EditableTextBox.h>
#include <Components/Button.h>
#include <GenericPlatform/GenericPlatformProcess.h>

UMALoginWidget::UMALoginWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UMALoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMALoginWidget::LoginButtonClicked()
{
	if (UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance(GetWorld())))
	{
		MAGameInstance->RequestLogin(UserIdText->GetText().ToString(), PasswordText->GetText().ToString());
		// TODO : 로그인 성공 시 캐릭터 선택 구현
		// 로그인 성공 시 델리게이트가 필요합니다.
	}
}

void UMALoginWidget::RegisterButtonClicked()
{
	// TODO : 회원 가입 주소 필요
	FPlatformProcess::LaunchURL(TEXT("http://google.com/"), NULL, NULL);
}
