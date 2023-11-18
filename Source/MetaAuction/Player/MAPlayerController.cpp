// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MAPlayerController.h"
#include "UI/MAHUDWidget.h"
#include "UI/MAAuctionWidget.h"
#include "UI/MAChatBubbleWidgetComponent.h"
#include "UI/MAConfirmCancelPopupWidget.h"
#include "UI/MAConfirmPopupWidget.h"
#include "Character/MACharacter.h"
#include "Core/MAGameState.h"
#include "Manager/ItemManager.h"
#include "UI/MAModelTransEditWidget.h"

#include <EngineUtils.h>



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

	// 테스트
	// if(IsRunningDedicatedServer())
	// 	return;
	//
	// FTransform trans;
	// trans.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
	//
	// FTimerHandle handle;
	// GetWorld()->GetTimerManager().SetTimer(handle, [this, trans]()
	// {
	// 	ServerRPC_SetModelRelativeTrans(MAGetMyJwtToken(GetGameInstance()), 7, trans);
	// }, 10.f, false);
	//////////////
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

	if (!IsLocalPlayerController())
	{
		return;
	}

	HUDWidget = CreateWidget<UMAHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
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
	AuctionWidget->AddToViewport();
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

void AMAPlayerController::SendChatLog(const FMAChatLogEntryData& InData)
{
	ServerSendChatLog(InData);
	ShowChatBubble(GetPawn(), InData);
}

/**
 *  모델 트랜스폼 변경 UI를 생성하여 띄웁니다.
 *  @param InItemLoc : 아이템의 위치
 *  @param InNowTransform : 현재 모델의 Relative Transform
 */
void AMAPlayerController::CreateModelTransEditWidget(const uint8 InItemLoc, const FTransform& InNowTransform)
{
	if(IsValid(TransEditWidgetPtr) && TransEditWidgetPtr->IsInViewport())
	{
		LOG_WARN(TEXT("already add viewport."));
		return;
	}

	TransEditWidgetPtr = CreateWidget<UMAModelTransEditWidget>(this, TransEditWidgetClass);

	if(IsValid(TransEditWidgetPtr))
	{
		// 위젯을 생성^^
		FInputModeUIOnly uiInput;
		SetInputMode(uiInput);
		SetShowMouseCursor(true);
		FlushPressedKeys();
	
		TransEditWidgetPtr->AddToViewport();
		TransEditWidgetPtr->PushData(InItemLoc, InNowTransform);
	}
}

/**
 *  Server RPC로 item actor에 배치된 모델의 상대적 transform을 변경을 요청합니다.
 *  @param InJwtToken : 요청한 사람의 토큰
 *  @param InItemLoc : 아이템의 위치
 *  @param InReleativeTrans : 변경할 transform
 */
void AMAPlayerController::ServerRPC_SetModelRelativeTrans_Implementation(const FString& InJwtToken, const uint8 InItemLoc, const FTransform& InReleativeTrans)
{
	LOG_WARN(TEXT("Server RPC"));
	if(const AMAGameState* gameState = Cast<AMAGameState>(GetWorld()->GetGameState()))
	{
		if(UItemManager* itemManager = gameState->GetItemManager())
		{
			itemManager->Server_SetModelTransform(InJwtToken, InItemLoc, InReleativeTrans);
		}
	}
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