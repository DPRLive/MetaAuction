// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemFilterWidget.h"
#include "Manager/ItemManager.h"

#include <GameFramework/GameState.h>
#include <Components/EditableText.h>
#include <Components/Button.h>
#include <Components/ComboBox.h>

UMAItemFilterWidget::UMAItemFilterWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemFilterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AGameState* GameState = IsValid(GetWorld()) ? GetWorld()->GetGameState<AGameState>() : nullptr;
	if (!ensure(GameState))
	{
		return;
	}
	UItemManager* ItemManager = GameState->GetComponentByClass<UItemManager>();
	if (!ensure(ItemManager))
	{
		return;
	}

	if (ensure(ItemDealTypeComboBox))
	{
		
	}
	if (ensure(ItemCanDealComboBox))
	{

	}
}
