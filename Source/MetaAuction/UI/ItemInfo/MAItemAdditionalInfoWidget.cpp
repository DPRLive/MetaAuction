﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/MAItemAdditionalInfoWidget.h"
#include "UI/ItemInfo/MAItemDisplayer.h"
#include "UI/Bidrecord/MABidRecordListWidget.h"
#include "UI/MAModelTransEditWidget.h"
#include "MetaAuction.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>

UMAItemAdditionalInfoWidget::UMAItemAdditionalInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);

	CommentCount = 0;
	bIsItemModelClicked = false;
}

void UMAItemAdditionalInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemModelImage);
	ensure(ModelTransEditButton);
	ensure(WBP_BidRecordList);

	if (IsValid(ItemModelImage))
	{
		ItemModelImage->OnMouseButtonDownEvent.BindUFunction(this, TEXT("ItemModelClicked"));
	}

	if (IsValid(ModelTransEditButton))
	{
		ModelTransEditButton->OnClicked.AddDynamic(this, &ThisClass::ModelTransEditButtonClicked);
		ModelTransEditButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMAItemAdditionalInfoWidget::NativeDestruct()
{
	if (SpawnedItemDisplayer.IsValid())
	{
		SpawnedItemDisplayer->Destroy();
		SpawnedItemDisplayer = nullptr;
	}

	if (SpawnedModelTransEditWidget.IsValid())
	{
		SpawnedModelTransEditWidget->RemoveFromParent();
	}

	Super::NativeDestruct();
}

FReply UMAItemAdditionalInfoWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsItemModelClicked = false;

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UMAItemAdditionalInfoWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsItemModelClicked)
	{
		FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
		SpawnedItemDisplayer->MoveCamera(StartMousePosition - MousePosition);
		StartMousePosition = MousePosition;
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UMAItemAdditionalInfoWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (SpawnedItemDisplayer.IsValid())
	{
		if (InMouseEvent.GetWheelDelta() < 0)
		{
			SpawnedItemDisplayer->ZoomOutCamera();
		}
		else if (InMouseEvent.GetWheelDelta() > 0)
		{
			SpawnedItemDisplayer->ZoomInCamera();
		}
	}
	
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UMAItemAdditionalInfoWidget::Update(const FItemData& InItemData)
{
	CachedItemData = InItemData;

	WBP_BidRecordList->Update(InItemData);

	// Update ModelImage
	if (SpawnedItemDisplayer.IsValid())
	{
		SpawnedItemDisplayer->Destroy();
		SpawnedItemDisplayer = nullptr;
	}

	if (InItemData.SellerName == MAGetMyUserName(MAGetGameInstance()))
	{
		ModelTransEditButton->SetVisibility(ESlateVisibility::Visible);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningPlayer();
	SpawnParams.Instigator = GetOwningPlayerPawn();
	SpawnedItemDisplayer = GetWorld()->SpawnActor<AMAItemDisplayer>(ItemDisplayerClass, ItemDisplayerTransform, SpawnParams);
	SpawnedItemDisplayer->Init(InItemData.ItemID);
}

FEventReply UMAItemAdditionalInfoWidget::ItemModelClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton ||
		MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		StartMousePosition = MouseEvent.GetScreenSpacePosition();
		bIsItemModelClicked = true;
	}

	return FEventReply();
}

void UMAItemAdditionalInfoWidget::ModelTransEditButtonClicked()
{
	if (!SpawnedModelTransEditWidget.IsValid())
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			SpawnedModelTransEditWidget = CreateWidget<UMAModelTransEditWidget>(PC, ModelTransEditWidgetClass);
			if (SpawnedModelTransEditWidget.IsValid())
			{
				SpawnedModelTransEditWidget->AddToViewport();
			}
		}
	}
}
