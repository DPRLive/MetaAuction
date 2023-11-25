// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MACharacterPickerTileWidget.h"
#include "UI/Title/MACharacterPickerEntry.h"

#include <Components/TileView.h>

UMACharacterPickerTileWidget::UMACharacterPickerTileWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UMACharacterPickerTileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Update();
}

void UMACharacterPickerTileWidget::Update()
{
	ItemTileView->ClearListItems();

	TArray<FCharacterMeshInfo>& CharacterMeshes = DA_MESH_INFO();
	for (int32 i = 0; i < CharacterMeshes.Num(); i++)
	{
		UMACharacterPickerEntry* Entry = NewObject<UMACharacterPickerEntry>();
		Entry->Index = i;
		Entry->Data = CharacterMeshes[i];
		ItemTileView->AddItem(Entry);
	}
}
