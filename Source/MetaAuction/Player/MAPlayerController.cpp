// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MAPlayerController.h"
#include "UI/MAHUDWidget.h"
#include "UI/MAAuctionWidget.h"
#include "UI/MAChatBubbleWidgetComponent.h"
#include "UI/MAConfirmCancelPopupWidget.h"
#include "UI/MAConfirmPopupWidget.h"
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
	if (HUDWidget)
	{
		return;
	}

	if (!HUDWidgetClass)
	{
		ensure(HUDWidgetClass);
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	HUDWidget = CreateWidget<UMAHUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AMAPlayerController::CreateAuctionWidget()
{
	if (AuctionWidget)
	{
		return;
	}

	if (!AuctionWidgetClass)
	{
		ensure(AuctionWidgetClass);
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	AuctionWidget = CreateWidget<UMAAuctionWidget>(this, AuctionWidgetClass);
	if (IsValid(AuctionWidget))
	{
		AuctionWidget->AddToViewport();
	}
}

void AMAPlayerController::SendChatLog(const FMAChatLogEntryData& InData)
{
	ServerSendChatLog(InData);
	ShowChatBubble(GetPawn(), InData);
}

UMAConfirmCancelPopupWidget* AMAPlayerController::CreateAndAddConfirmCancelPopupWidget()
{
	UMAConfirmCancelPopupWidget* PopupWidget = CreateWidget<UMAConfirmCancelPopupWidget>(this, ConfirmCancelPopupWidgetClass);
	if (IsValid(PopupWidget))
	{
		PopupWidget->AddToViewport();
	}
	return PopupWidget;
}

UMAConfirmPopupWidget* AMAPlayerController::CreateAndAddConfirmPopupWidget()
{
	UMAConfirmPopupWidget* PopupWidget = CreateWidget<UMAConfirmPopupWidget>(this, ConfirmPopupWidgetClass);
	if (IsValid(PopupWidget))
	{
		PopupWidget->AddToViewport();
	}
	return PopupWidget;
}

void AMAPlayerController::ServerSendChatLog_Implementation(const FMAChatLogEntryData& InData)
{
	for (AMAPlayerController* MAPC : TActorRange<AMAPlayerController>(GetWorld()))
	{
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