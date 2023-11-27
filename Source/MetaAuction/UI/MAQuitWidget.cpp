// Fill out your copyright notice in the Description page of Project Settings.


#include "MAQuitWidget.h"
#include "Player/MAPlayerControllerBase.h"

#include <Kismet/KismetSystemLibrary.h>
#include <Components/Button.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAQuitWidget)

UMAQuitWidget::UMAQuitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMAQuitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QuitGameBtn->OnClicked.AddDynamic(this, &UMAQuitWidget::_OnClickQuitGameBtn);
	TravelLobbyBtn->OnClicked.AddDynamic(this, &UMAQuitWidget::_OnClickTravelLobbyBtn);
}

/**
 *	게임을 종료합니다.
 */
void UMAQuitWidget::_OnClickQuitGameBtn()
{
	// 종료 전 안 쓸것 같은 캐시 파일을 한번 지워줍니다.
	if(FItemFileHandler* fileHandler = MAGetItemFileHandler(GetGameInstance()))
		fileHandler->RemoveCacheFile(ERemoveCacheType::Unuseable);
	
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
	RemoveFromParent();
}

/**
 *	로비로 이동합니다.
 */
void UMAQuitWidget::_OnClickTravelLobbyBtn()
{
	if(AMAPlayerControllerBase* controller = Cast<AMAPlayerControllerBase>(GetOwningPlayer()))
	{
		RemoveFromParent();
		controller->ClientLevelTravel(ELevelType::Lobby);
	}
}