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

	UFUNCTION(BlueprintCallable, Category = "UI|ChatLog")
	void SendChatLog(const FMAChatLogEntryData& Data);

	UFUNCTION(BlueprintCallable, Category = "UI|Popup")
	class UMAConfirmCancelPopupWidget* CreateAndAddConfirmCancelPopupWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|Popup")
	class UMAConfirmPopupWidget* CreateAndAddConfirmPopupWidget();

private:

	UFUNCTION(Server, Reliable, Category = "UI|ChatLog")
	void ServerSendChatLog(const FMAChatLogEntryData& InData);

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

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Popup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMAConfirmCancelPopupWidget> ConfirmCancelPopupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Popup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMAConfirmPopupWidget> ConfirmPopupWidgetClass;
};
