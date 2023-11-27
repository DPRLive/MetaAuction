// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/PlayerController.h>
#include "MAPlayerControllerBase.generated.h"

class UInputAction;
class UMAQuitWidget;
/**
 * PlayerController의 Base 클래스 입니다.
 */
UCLASS()
class METAAUCTION_API AMAPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	// Client를 레벨 이동 시킵니다. 경매장으로 이동 시 로그인이 되어있지 않으면 이동시키지 않습니다.
	UFUNCTION( BlueprintCallable, Category = "Travel")
	void ClientLevelTravel(const ELevelType InType);

	// QuitWidget을 띄웁니다.
	UFUNCTION( BlueprintCallable )
	void OpenQuitWidget();
	
private:
	// Quit Widget입니다.
	UPROPERTY( EditDefaultsOnly, Category = "UI|Quit", meta = (AllowPrivateAccess = true) )
	TSubclassOf<UMAQuitWidget> QuitWidgetClass;

	UPROPERTY()
	TObjectPtr<UMAQuitWidget> QuitWidgetPtr;
};
