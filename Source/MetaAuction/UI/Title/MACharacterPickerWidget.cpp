// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MACharacterPickerWidget.h"
#include "UI/Title/MACharacterPickerTileWidget.h"
#include "UI/Title/MACharacterPickerEntry.h"
#include "Core/MAPlayerState.h"
#include "Player/MAPlayerControllerBase.h"

#include <Components/Button.h>
#include <Components/TileView.h>


UMACharacterPickerWidget::UMACharacterPickerWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UMACharacterPickerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);
}

void UMACharacterPickerWidget::ConfirmButtonClicked()
{
	if (UMACharacterPickerEntry* SelectedEntry = WBP_CharacterPickerTile->GetTileView()->GetSelectedItem<UMACharacterPickerEntry>())
	{
		if (AMAPlayerState* MAPlayerState = Cast<AMAPlayerState>(GetOwningPlayerState()))
		{
			MAPlayerState->ServerRPC_SelectCharacter(SelectedEntry->Index);

			// InputModeGameOnly
			FInputModeGameOnly InputModeGameOnly;
			GetOwningPlayer()->SetInputMode(InputModeGameOnly);
			GetOwningPlayer()->SetShowMouseCursor(false);
			GetOwningPlayer()->FlushPressedKeys();
			RemoveFromParent();

			// 컨트롤러를 Travel 시킵니다.
			if(AMAPlayerControllerBase* localController = Cast<AMAPlayerControllerBase>(GetOwningPlayer()))
			{
				localController->ClientLevelTravel(ELevelType::Auction);
			}
		}
	}
}
