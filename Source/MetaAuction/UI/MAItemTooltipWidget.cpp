// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemTooltipWidget.h"
#include "UI/MAWidgetUtils.h"
#include "Handler/ItemFileHandler.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>
#include <ImageUtils.h>

UMAItemTooltipWidget::UMAItemTooltipWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentImageIndex = 0;
	MaxImageIndex = 0;
}

void UMAItemTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemImagePrevButton);
	if (IsValid(ItemImagePrevButton))
	{
		ItemImagePrevButton->OnClicked.AddDynamic(this, &ThisClass::ItemImagePrevButtonClicked);
	}
	
	ensure(ItemImageNextButton);
	if (IsValid(ItemImageNextButton))
	{
		ItemImageNextButton->OnClicked.AddDynamic(this, &ThisClass::ItemImageNextButtonClicked);
	}
}

void UMAItemTooltipWidget::UpdateById(uint32 InItemID)
{
	UItemManager* ItemManager = UMAWidgetUtils::GetItemManager(GetWorld());
	if (!IsValid(ItemManager))
	{
		return;
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto Func = [ThisPtr](const FItemData& InData)
			{
				if (ThisPtr.IsValid())
				{
					ThisPtr->UpdateAll(InData);
				}
			};
		ItemManager->RequestItemDataByID(Func, InItemID);
	}
}

void UMAItemTooltipWidget::UpdateAll(const FItemData& InItemData)
{
	UpdateText(InItemData);
	UpdateImage(InItemData);

	CachedItemData = InItemData;
	CurrentImageIndex = 0;
	MaxImageIndex = InItemData.ImgCount;
}

void UMAItemTooltipWidget::UpdateText(const FItemData& InItemData)
{
	TitleText->SetText(FText::FromString(InItemData.Title));
	InformationText->SetText(FText::FromString(InItemData.Information));
	BuyerNameText->SetText(FText::FromString(InItemData.BuyerName));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true);

	StartPriceText->SetText(FText::AsNumber(InItemData.StartPrice, &NumberFormatOptions));
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));

	const int32 MaxEndTimeIndex = 5;
	//const TArray<uint16>& EndTime = InItemData.EndTime;
	// TArray<uint16> TempTime(0, MaxEndTimeIndex);
	// for (int32 i = 0; i < MaxEndTimeIndex; i++)
	// {
	// 	if (EndTime.IsValidIndex(i))
	// 	{
	// 		TempTime[i] = EndTime[i];
	// 	}
	// }
	// FString EndTimeString = FString::Printf(TEXT("%04d.%02d.%02d.%02d.%02d"), TempTime[0], TempTime[1], TempTime[2], TempTime[3], TempTime[4]);
	// EndTimeText->SetText(FText::FromString(EndTimeString));
}

void UMAItemTooltipWidget::UpdateImage(const FItemData& InItemData)
{
	TWeakPtr<FItemFileHandler> ItemFileHandler = UMAWidgetUtils::GetItemFileHandler(GetWorld());
	if (ItemFileHandler.IsValid())
	{
		LOG_SCREEN(FColor::Green, TEXT("Request %s"), *FString(__FUNCTION__));
		TWeakObjectPtr<ThisClass> ThisPtr(this);
		if (ThisPtr.IsValid())
		{
			auto Func = [ThisPtr](UTexture2DDynamic* InImage)
				{
					if (ThisPtr.IsValid())
					{
						ThisPtr->ItemImage->SetBrushFromTextureDynamic(InImage);
						LOG_SCREEN(FColor::Green, TEXT("Successed %s"), *FString(__FUNCTION__));
					}
				};
			ItemFileHandler.Pin()->RequestImg(Func, InItemData.ItemID, CurrentImageIndex);
		}
	}
}

void UMAItemTooltipWidget::ItemImagePrevButtonClicked()
{
	if (CurrentImageIndex > 0)
	{
		CurrentImageIndex--;
		UpdateImage(CachedItemData);
	}
}

void UMAItemTooltipWidget::ItemImageNextButtonClicked()
{
	if (CurrentImageIndex < MaxImageIndex)
	{
		CurrentImageIndex++;
		UpdateImage(CachedItemData);
	}
}