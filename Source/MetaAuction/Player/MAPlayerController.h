// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/MAChatLogEntry.h"
#include "MAPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatLogEventSignature, const FMAChatLogEntryData&, InData);

/**
 * 
 */
UCLASS()
class METAAUCTION_API AMAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AMAPlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	FORCEINLINE class UMAHUDWidget* GetHUDWidget() const { return HUDWidget; }
	void CreateHUDWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|Auction")
	FORCEINLINE class UMAAuctionWidget* GetAuctionWidget() const { return AuctionWidget; }
	void CreateAuctionWidget();

	// RPC ó���� �� ChatLog�� ������ �� ����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "UI|ChatLog")
	void SendChatLog(const FMAChatLogEntryData& Data);

private:

	// Client Widget���κ��� Server�� ChatLog�� ���� �� ���˴ϴ�.
	UFUNCTION(Server, Reliable, Category = "UI|ChatLog")
	void ServerSendChatLog(const FMAChatLogEntryData& InData);

	// �������� ���� ChatLog�� Client���� ������ �� ����մϴ�.
	UFUNCTION(Client, Reliable, Category = "UI|ChatLog")
	void ClientReceiveChatLog(class APawn* SourcePawn, const FMAChatLogEntryData& InData);

	void ShowChatBubble(class APawn* SourcePawn, const FMAChatLogEntryData& InData);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TSubclassOf<class UMAHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UI|HUD")
	TObjectPtr<class UMAHUDWidget> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Auction")
	TSubclassOf<class UMAAuctionWidget> AuctionWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UI|Auction")
	TObjectPtr<class UMAAuctionWidget> AuctionWidget;

	UPROPERTY(BlueprintAssignable, Category = "UI|ChatLog")
	FChatLogEventSignature OnReceivedChatLog;
	
	// HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
	void RequestBid(uint32 InItemID, uint64 InPrice);
};
