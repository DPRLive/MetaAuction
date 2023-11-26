// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/MACharacterPickerEntryWidget.h"
#include "UI/Title/MACharacterPickerEntry.h"

#include <Components/Image.h>
#include <Engine/Texture2D.h>

UMACharacterPickerEntryWidget::UMACharacterPickerEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HoveredColor = FLinearColor::White;
	SelectedColor = FLinearColor::White;
}

void UMACharacterPickerEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NormalColor = CharacterImage->GetColorAndOpacity();
}

void UMACharacterPickerEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMACharacterPickerEntry* Entry = Cast<UMACharacterPickerEntry>(ListItemObject))
	{
		if (Entry->Data.MeshImg.IsPending())
		{
			Entry->Data.MeshImg.LoadSynchronous();
		}

		CharacterImage->SetBrushFromTexture(Entry->Data.MeshImg.Get());
	}
}

void UMACharacterPickerEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Hover();
}

void UMACharacterPickerEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Unhover();
}

void UMACharacterPickerEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (bIsSelected)
	{
		Select();
	}
	else
	{
		Unselect();
	}
}

void UMACharacterPickerEntryWidget::Hover()
{
	if (!IsListItemSelected())
	{
		CharacterImage->SetColorAndOpacity(HoveredColor);
	}
}

void UMACharacterPickerEntryWidget::Unhover()
{
	if (!IsListItemSelected())
	{
		CharacterImage->SetColorAndOpacity(NormalColor);
	}
}

void UMACharacterPickerEntryWidget::Select()
{
	CharacterImage->SetColorAndOpacity(SelectedColor);
}

void UMACharacterPickerEntryWidget::Unselect()
{
	if (IsHovered())
	{
		CharacterImage->SetColorAndOpacity(NormalColor);
	}
	else
	{
		CharacterImage->SetColorAndOpacity(NormalColor);
	}
}