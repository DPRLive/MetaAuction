// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MAPlayerController.h"
#include "UI/MAHUDWidget.h"
#include "UI/MAAuctionWidget.h"
#include "UI/Chat/MAChatBubbleWidgetComponent.h"
#include "UI/Common/MAConfirmCancelPopupWidget.h"
#include "UI/Common/MAConfirmPopupWidget.h"
#include "UI/ItemInfo/MAItemInfoWidget.h"
#include "Character/MACharacter.h"
#include "Core/MAGameState.h"
#include "Manager/ItemManager.h"

#include <EngineUtils.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAPlayerController)

AMAPlayerController::AMAPlayerController()
{
}

void AMAPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
 *  아이템 정보 UI를 생성하여 띄웁니다.
 *  @param InItemId : 볼 아이템의 id, uint32 범위로 넣어야함
 */
void AMAPlayerController::CreateItemInfoWidget(const int64 InItemId)
{
	if(InItemId <= 0)
		return;
	
	if(IsValid(ItemInfoWidgetPtr) && ItemInfoWidgetPtr->IsInViewport())
	{
		LOG_WARN(TEXT("already add viewport."));
		return;
	}

	ItemInfoWidgetPtr = CreateWidget<UMAItemInfoWidget>(this, ItemInfoWidgetClass);

	if(IsValid(ItemInfoWidgetPtr))
	{
		// 위젯을 생성^^
		FInputModeUIOnly uiInput;
		SetInputMode(uiInput);
		SetShowMouseCursor(true);
		FlushPressedKeys();
	
		ItemInfoWidgetPtr->AddToViewport();
	}

	// 데이터를 넣는다.
	if (const UItemDataHandler* itemDataHandler = MAGetItemDataHandler(GetWorld()->GetGameState()))
	{
		TWeakObjectPtr<const AMAPlayerController> thisPtr = this;
		itemDataHandler->RequestItemDataById([thisPtr](const FItemData& InItemData)
		{
			if (IsValid(thisPtr->ItemInfoWidgetPtr))
			{
				thisPtr->ItemInfoWidgetPtr->Update(InItemData);
			}
		}, InItemId);
	}
}

/**
 *  Client를 레벨 이동 시킵니다. 경매장으로 이동 시 로그인이 되어있지 않으면 이동시키지 않습니다.
 *  @param InType : 이동할 레벨의 타입
 */
void AMAPlayerController::ClientLevelTravel(const ELevelType InType)
{
	FString Url = TEXT("");

	if(InType == ELevelType::Lobby)
		Url = DA_NETWORK(LobbyUrl);
	else if(InType == ELevelType::Auction && (MAGetMyJwtToken(GetGameInstance()) != TEXT("")))
		Url = DA_NETWORK(AuctionUrl);
	
	if(Url == TEXT(""))
		return;
	
	ClientTravel(Url, TRAVEL_Absolute, false);
}

/**
 *  Server RPC로 item actor에 배치된 모델의 상대적 transform을 변경을 요청합니다.
 *  @param InJwtToken : 요청한 사람의 토큰
 *  @param InItemLoc : 아이템의 위치
 *  @param InRelativeTrans : 변경할 transform
 */
void AMAPlayerController::ServerRPC_SetModelRelativeTrans_Implementation(const FString& InJwtToken, const uint8 InItemLoc, const FTransform& InRelativeTrans)
{
	LOG_WARN(TEXT("Server RPC"));
	if(const AMAGameState* gameState = Cast<AMAGameState>(GetWorld()->GetGameState()))
	{
		if(UItemManager* itemManager = gameState->GetItemManager())
		{
			itemManager->Server_SetModelTransform(InJwtToken, InItemLoc, InRelativeTrans);
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