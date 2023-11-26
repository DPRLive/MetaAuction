// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MACharacterPickerWidget.h"
#include "UI/Title/MACharacterPickerTileWidget.h"
#include "UI/Title/MACharacterPickerEntry.h"
#include "Core/MAPlayerState.h"

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

			LOG_WARN(TEXT("ClientTravelToSession을 호출해 주세요."));
			// TODO : ClientTravelToSession 호출하기
			// MAGetGameInstance(GetWorld())->ClientTravelToSession();

			RemoveFromParent();
		}
	}
}
