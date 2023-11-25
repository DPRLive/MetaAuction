// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MALoginWidget.h"
#include "UI/Title/MACharacterPickerWidget.h"
#include "Core/MAGameInstance.h"

#include <Components/EditableTextBox.h>
#include <Components/Button.h>
#include <GenericPlatform/GenericPlatformProcess.h>

UMALoginWidget::UMALoginWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UMALoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// InputModeUIOnly
	FInputModeUIOnly InputModeUIOnly;
	GetOwningPlayer()->SetInputMode(InputModeUIOnly);
	GetOwningPlayer()->SetShowMouseCursor(true);
	GetOwningPlayer()->FlushPressedKeys();

	LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginButtonClicked);
	RegisterButton->OnClicked.AddDynamic(this, &ThisClass::RegisterButtonClicked);

	// 로그인 성공 시 UI 닫기 및 캐릭터 선택창 띄우기
	if (UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance(GetWorld())))
	{
		TWeakObjectPtr<ThisClass> ThisPtr;
		MAGameInstance->OnLoginDelegate.Remove(OnLoginDelegateHandle);
		OnLoginDelegateHandle = MAGameInstance->OnLoginDelegate.AddLambda([ThisPtr](bool bIsSuccessed)
			{
				if (ThisPtr.IsValid() && bIsSuccessed)
				{
					if (UMACharacterPickerWidget* CharacterPickerWidget = CreateWidget<UMACharacterPickerWidget>(ThisPtr->GetOwningPlayer(), ThisPtr->CharacterPickerWidgetClass))
					{
						CharacterPickerWidget->AddToViewport();
						ThisPtr->RemoveFromParent();
					}
				}
			});
	}
}

void UMALoginWidget::NativeDestruct()
{
	if (UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance(GetWorld())))
	{
		MAGameInstance->OnLoginDelegate.Remove(OnLoginDelegateHandle);
	}
	
	Super::NativeDestruct();
}

void UMALoginWidget::LoginButtonClicked()
{
	if (UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance(GetWorld())))
	{
		MAGameInstance->RequestLogin(UserIdText->GetText().ToString(), PasswordText->GetText().ToString());
	}
}

void UMALoginWidget::RegisterButtonClicked()
{
	// TODO : 회원 가입 주소 필요
	FPlatformProcess::LaunchURL(TEXT("http://google.com/"), NULL, NULL);
}
