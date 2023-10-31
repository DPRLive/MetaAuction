// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemInfoWidget.h"
#include "UI/MAItemImageEntry.h"
#include "UI/MAItemImageListWidget.h"
#include "UI/MAItemAdditionalInfoWidget.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/ListView.h>
#include <ImageUtils.h>

UMAItemInfoWidget::UMAItemInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UMAItemInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemImage);
	ensure(TitleText);
	ensure(BuyerNameText);
	ensure(StartPriceText);
	ensure(CurrentPriceText);
	ensure(EndTimeText);
	ensure(ItemImagePrevButton);
	ensure(ItemImageNextButton);
	ensure(WBP_ItemImageList);
	ensure(DetailsButton);

	if (IsValid(ItemImagePrevButton))
	{
		ItemImagePrevButton->OnClicked.AddDynamic(this, &ThisClass::ItemImagePrevButtonClicked);
	}

	if (IsValid(ItemImageNextButton))
	{
		ItemImageNextButton->OnClicked.AddDynamic(this, &ThisClass::ItemImageNextButtonClicked);
	}

	if (IsValid(WBP_ItemImageList) && IsValid(ItemImage))
	{
		TWeakObjectPtr<ThisClass> ThisPtr(this);
		if (ThisPtr.IsValid())
		{
			auto Func = [ThisPtr](UObject* Object)
				{
					UMAItemImageEntry* Entry = Cast<UMAItemImageEntry>(Object);
					if (ThisPtr.IsValid() && IsValid(Entry))
					{
						ThisPtr->ItemImage->SetBrushFromTextureDynamic(Entry->Data.Image);
					}
				};
			WBP_ItemImageList->GetListView()->OnItemSelectionChanged().Remove(ItemImageListViewSelectionChangedHandle);
			ItemImageListViewSelectionChangedHandle = WBP_ItemImageList->GetListView()->OnItemSelectionChanged().AddLambda(Func);
		}
	}

	if (IsValid(DetailsButton))
	{
		DetailsButton->OnClicked.AddDynamic(this, &ThisClass::DetailsButtonClicked);
	}
}

void UMAItemInfoWidget::Update(const FItemData& InItemData)
{
	TitleText->SetText(FText::FromString(InItemData.Title));
	BuyerNameText->SetText(FText::FromString(InItemData.BuyerName));

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true);

	StartPriceText->SetText(FText::AsNumber(InItemData.StartPrice, &NumberFormatOptions));
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));

	const int32 MaxEndTimeIndex = 5;

	WBP_ItemImageList->Update(InItemData);
	UMAItemImageEntry* Entry = WBP_ItemImageList->GetSelectedItemImageEntry();
	if (IsValid(Entry))
	{
		ItemImage->SetBrushFromTextureDynamic(Entry->Data.Image);
	}

	CachedItemData = InItemData;
}

void UMAItemInfoWidget::ItemImagePrevButtonClicked()
{
	if (IsValid(WBP_ItemImageList))
	{
		WBP_ItemImageList->SelectPrevItem();
	}
}

void UMAItemInfoWidget::ItemImageNextButtonClicked()
{
	if (IsValid(WBP_ItemImageList))
	{
		WBP_ItemImageList->SelectNextItem();
	}
}

void UMAItemInfoWidget::DetailsButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		return;
	}

	UMAItemAdditionalInfoWidget* ItemAdditionalInfoWidget = CreateWidget<UMAItemAdditionalInfoWidget>(PC, ItemAdditionalInfoWidgetClass);
	if (IsValid(ItemAdditionalInfoWidget))
	{
		ItemAdditionalInfoWidget->AddToViewport();
		ItemAdditionalInfoWidget->Update(CachedItemData);
	}
}