// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MANameplateWidget.h"
#include "Core/MAGameInstance.h"
#include "Data/LoginData.h"
#include "Components/TextBlock.h"

UMANameplateWidget::UMANameplateWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMANameplateWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(NameText);
}

void UMANameplateWidget::Update()
{
	if (IsValid(NameText))
	{
		UMAGameInstance* MAGameInstance = Cast<UMAGameInstance>(MAGetGameInstance());
		if (IsValid(MAGameInstance) && MAGameInstance->GetLoginData().IsValid())
		{
			NameText->SetText(FText::FromString(MAGameInstance->GetLoginData()->GetUserName()));
		}
	}
}
