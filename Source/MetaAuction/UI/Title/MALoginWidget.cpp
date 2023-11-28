// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MALoginWidget.h"
#include "UI/Title/MACharacterPickerWidget.h"
#include "UI/Common/MAConfirmPopupWidget.h"
#include "Core/MAGameInstance.h"
#include "Player/MAPlayerControllerBase.h"

#include <Components/EditableTextBox.h>
#include <Components/Button.h>
#include <GenericPlatform/GenericPlatformProcess.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MALoginWidget)

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

	if (UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance(GetWorld())))
	{
		TWeakObjectPtr<ThisClass> ThisPtr = this;
		MAGameInstance->OnLoginDelegate.Remove(OnLoginDelegateHandle);
		OnLoginDelegateHandle = MAGameInstance->OnLoginDelegate.AddLambda([ThisPtr](bool bIsSuccessed)
			{
				if (ThisPtr.IsValid())
				{
					// 로그인 성공 시 위젯 파괴
					if (bIsSuccessed)
					{
						ThisPtr->RemoveFromParent();
					}
					// 로그인 실패 시 결과 팝업 창 생성
					else
					{
						if (AMAPlayerControllerBase* MAPC = Cast<AMAPlayerControllerBase>(ThisPtr->GetOwningPlayer()))
						{
							if (UMAConfirmPopupWidget* PopupWidget = MAPC->CreateAndAddConfirmPopupWidget())
							{
								PopupWidget->SetText(TEXT("로그인에 실패하였습니다."));
							}
						}
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

	// 위젯 파괴 시 캐릭터 선택 창 띄우기
	if (UMACharacterPickerWidget* CharacterPickerWidget = CreateWidget<UMACharacterPickerWidget>(GetOwningPlayer(), CharacterPickerWidgetClass))
	{
		CharacterPickerWidget->AddToViewport();
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
	FPlatformProcess::LaunchURL(*DA_NETWORK(SignUpUrl), nullptr, nullptr);
}
