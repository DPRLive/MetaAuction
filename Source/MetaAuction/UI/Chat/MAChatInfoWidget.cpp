// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatInfoWidget.h"
#include "UI/Chat/MAChatRoomListWidget.h"

UMAChatInfoWidget::UMAChatInfoWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UMAChatInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(WBP_ChatRoomList);
}

void UMAChatInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMAChatInfoWidget::Update()
{
	WBP_ChatRoomList->Update();
}
