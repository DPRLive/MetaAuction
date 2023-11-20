// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/MAItemAdditionalInfoWidget.h"
#include "UI/ItemInfo/MAItemDisplayer.h"
#include "UI/Bidrecord/MABidRecordListWidget.h"
#include "MetaAuction.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>

UMAItemAdditionalInfoWidget::UMAItemAdditionalInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommentCount = 0;

	SetIsFocusable(true);
}

void UMAItemAdditionalInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemModelImage);
	ensure(WBP_BidRecordList);
}

void UMAItemAdditionalInfoWidget::NativeDestruct()
{
	if (SpawnedItemDisplayer.IsValid())
	{
		SpawnedItemDisplayer->Destroy();
		SpawnedItemDisplayer = nullptr;
	}

	Super::NativeDestruct();
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

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningPlayer();
	SpawnParams.Instigator = GetOwningPlayerPawn();
	SpawnedItemDisplayer = GetWorld()->SpawnActor<AMAItemDisplayer>(ItemDisplayerClass, ItemDisplayerTransform, SpawnParams);
	SpawnedItemDisplayer->Init(InItemData.ItemID);
}