// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/MAItemImageEntryWidget.h"
#include "UI/ItemInfo/MAItemImageEntry.h"

#include <Components/Image.h>

UMAItemImageEntryWidget::UMAItemImageEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HoveredColor = FLinearColor::White;
	SelectedColor = FLinearColor::White;
}

void UMAItemImageEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NormalColor = ItemImage->GetColorAndOpacity();
}

void UMAItemImageEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Hover();
}

void UMAItemImageEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Unhover();
}

void UMAItemImageEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UMAItemImageEntry* ItemImageEntry = Cast<UMAItemImageEntry>(ListItemObject);
	ensure(ItemImageEntry);
	if (nullptr == ItemImageEntry)
	{
		return;
	}

	ItemImage->SetBrushFromTextureDynamic(ItemImageEntry->Data.Image);
}

void UMAItemImageEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
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

void UMAItemImageEntryWidget::Hover()
{
	if (!IsListItemSelected())
	{
		ItemImage->SetColorAndOpacity(HoveredColor);
	}
}

void UMAItemImageEntryWidget::Unhover()
{
	if (!IsListItemSelected())
	{
		ItemImage->SetColorAndOpacity(NormalColor);
	}
}

void UMAItemImageEntryWidget::Select()
{
	ItemImage->SetColorAndOpacity(SelectedColor);
}

void UMAItemImageEntryWidget::Unselect()
{
	if (IsHovered())
	{
		ItemImage->SetColorAndOpacity(NormalColor);
	}
	else
	{
		ItemImage->SetColorAndOpacity(NormalColor);
	}
}