// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "MAQuitWidget.generated.h"

class UButton;
/**
 * ESC를 누르면 튀어나올 Quit widget 입니다.
 */
UCLASS()
class METAAUCTION_API UMAQuitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMAQuitWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

private:
	// 게임 종료 버튼이 눌리면 호출됩니다.
	UFUNCTION()
	void _OnClickQuitGameBtn();

	UFUNCTION()
	void _OnClickTravelLobbyBtn();
	
	// 게임 종료 버튼
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> QuitGameBtn;

	// 로비로 이동 버튼
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> TravelLobbyBtn;
};
