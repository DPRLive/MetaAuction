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

/**
 * HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
 * @param InItemID : 입찰할 item의 id
 * @param InPrice : 입찰할 가격
 */
void AMAPlayerController::RequestBid(uint32 InItemID, uint64 InPrice)
{
	if(IsRunningDedicatedServer()) return; // 데디면 하지 마라.
	
	// Body를 만든다.
	FString requestBody = FString::Printf(TEXT("%llu"), InPrice);
	
	if (const FHttpHandler* httpHandler = MAGetHttpHandler(GetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		// 혹시 모르니 나를 소유한 놈이 나를 잘 가리키고 있는지 확인하기 위해 Weak 캡처 추가.
		TWeakObjectPtr<AMAPlayerController> thisPtr = this;
		httpHandler->Request(DA_NETWORK(BidAddURL) + FString::Printf(TEXT("/%d"), InItemID), EHttpRequestType::POST,[thisPtr](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid())
								 {
									 if(EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
									 {
										 // 입찰 성공
										 LOG_WARN(TEXT("Bid Success!"));
									 }
									else
									{
										// 입찰 실패, InResponse->GetContentAsString() : 서버에서 알려준 입찰 실패 이유
										LOG_WARN(TEXT("bid Failed!"));
										// Json reader 생성
										UE_LOG(LogTemp, Warning, TEXT("%s"), *InResponse->GetContentAsString());
									}
								 }
							 }, requestBody);
	}
}