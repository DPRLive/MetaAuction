// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MAPlayerController.h"
#include "UI/MAHUDWidget.h"
#include "UI/MAAuctionWidget.h"
#include "UI/MAChatBubbleWidgetComponent.h"
#include "Character/MACharacter.h"

#include <EngineUtils.h>
#include <Interfaces/IHttpResponse.h>

AMAPlayerController::AMAPlayerController()
{
}

void AMAPlayerController::BeginPlay()
{
	CreateHUDWidget();
	CreateAuctionWidget();
	if (AuctionWidget)
	{
		AuctionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMAPlayerController::CreateHUDWidget()
{
	// �ϳ��� ������ �����ϵ��� �մϴ�.
	if (HUDWidget)
	{
		return;
	}

	// Ŭ������ ��ȿ���� Ȯ���մϴ�.
	if (!HUDWidgetClass)
	{
		ensure(HUDWidgetClass);
		return;
	}

	// ���� �÷��̾� ��Ʈ�ѷ������� ����ϴ�.
	if (!IsLocalPlayerController())
	{
		return;
	}

	HUDWidget = CreateWidget<UMAHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
}

void AMAPlayerController::CreateAuctionWidget()
{
	// �ϳ��� ������ �����ϵ��� �մϴ�.
	if (AuctionWidget)
	{
		return;
	}

	// Ŭ������ ��ȿ���� Ȯ���մϴ�.
	if (!AuctionWidgetClass)
	{
		ensure(AuctionWidgetClass);
		return;
	}

	// ���� �÷��̾� ��Ʈ�ѷ������� ����ϴ�.
	if (!IsLocalPlayerController())
	{
		return;
	}

	AuctionWidget = CreateWidget<UMAAuctionWidget>(this, AuctionWidgetClass);
	AuctionWidget->AddToViewport();
}

void AMAPlayerController::SendChatLog(const FMAChatLogEntryData& InData)
{
	ServerSendChatLog(InData);
	ShowChatBubble(GetPawn(), InData);
}

void AMAPlayerController::ServerSendChatLog_Implementation(const FMAChatLogEntryData& InData)
{
	for (AMAPlayerController* MAPC : TActorRange<AMAPlayerController>(GetWorld()))
	{
		// ������ �ƴϰ� �޽����� ���� �÷��̾� ��Ʈ�ѷ��� �ƴ� ��Ʈ�ѷ���
		if (IsValid(MAPC) && this != MAPC && !MAPC->IsLocalController())
		{
			MAPC->ClientReceiveChatLog(GetPawn(), InData);
		}
	}
}

void AMAPlayerController::ClientReceiveChatLog_Implementation(APawn* SourcePawn, const FMAChatLogEntryData& InData)
{
	ShowChatBubble(SourcePawn, InData);
	OnReceivedChatLog.Broadcast(InData);
}

void AMAPlayerController::ShowChatBubble(APawn* SourcePawn, const FMAChatLogEntryData& InData)
{
	// ChatBubble �ִϸ��̼� ����ϱ�
	AMACharacter* MACharacter = Cast<AMACharacter>(SourcePawn);
	if (IsValid(MACharacter))
	{
		UMAChatBubbleWidgetComponent* ChatBubbleWidgetComponent = MACharacter->GetComponentByClass<UMAChatBubbleWidgetComponent>();
		if (IsValid(ChatBubbleWidgetComponent))
		{
			ChatBubbleWidgetComponent->ShowChatBubble(InData.ChatLog.ToString());
		}
	}
}