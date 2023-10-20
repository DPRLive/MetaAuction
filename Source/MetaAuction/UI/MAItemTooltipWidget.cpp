// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemTooltipWidget.h"
#include "Manager/ItemManager.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>
#include <GameFramework/GameState.h>
#include <ImageUtils.h>

UMAItemTooltipWidget::UMAItemTooltipWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentImageIndex = 0;
}

void UMAItemTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(ItemImagePrevButton))
	{
		ItemImagePrevButton->OnClicked.AddDynamic(this, &ThisClass::ItemImagePrevButtonClicked);
	}

	if (ensure(ItemImageNextButton))
	{
		ItemImageNextButton->OnClicked.AddDynamic(this, &ThisClass::ItemImageNextButtonClicked);
	}
}

void UMAItemTooltipWidget::UpdateById(uint32 InItemID)
{
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

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (ThisPtr.IsValid())
	{
		auto GetItemDataByID = [ThisPtr](const FItemData& InData)
			{
				ThisPtr->UpdateAll(InData);
			};
		ItemManager->RequestItemDataByID(GetItemDataByID, InItemID);
	}
}

void UMAItemTooltipWidget::UpdateAll(const FItemData& InItemData)
{
	UpdateText(InItemData);
	LoadAllImage(InItemData);
	UpdateImage();
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
	const TArray<uint16>& EndTime = InItemData.EndTime;
	TArray<uint16> TempTime(0, MaxEndTimeIndex);
	for (int32 i = 0; i < MaxEndTimeIndex; i++)
	{
		if (EndTime.IsValidIndex(i))
		{
			TempTime[i] = EndTime[i];
		}
	}
	FString EndTimeString = FString::Printf(TEXT("%04d.%02d.%02d.%02d.%02d"), TempTime[0], TempTime[1], TempTime[2], TempTime[3], TempTime[4]);
	EndTimeText->SetText(FText::FromString(EndTimeString));
}

void UMAItemTooltipWidget::UpdateImage()
{
	if (CachedTextures.IsValidIndex(CurrentImageIndex))
	{
		ItemImage->SetBrushFromTexture(CachedTextures[CurrentImageIndex], false);
	}
}

void UMAItemTooltipWidget::ItemImagePrevButtonClicked()
{
	if (CurrentImageIndex > 0)
	{
		CurrentImageIndex--;
		UpdateImage();
	}
}

void UMAItemTooltipWidget::ItemImageNextButtonClicked()
{
	if (CurrentImageIndex < CachedTextures.Num())
	{
		CurrentImageIndex++;
		UpdateImage();
	}
}

void UMAItemTooltipWidget::LoadAllImage(const FItemData& InItemData)
{
	// AGameState* GameState = IsValid(GetWorld()) ? GetWorld()->GetGameState<AGameState>() : nullptr;
	// if (!IsValid(GameState))
	// {
	// 	return;
	// }
	// UItemManager* ItemManager = GameState->GetComponentByClass<UItemManager>();
	// if (!IsValid(ItemManager))
	// {
	// 	return;
	// }
}

void UMAItemTooltipWidget::AddImage(const FString& InFilePath)
{
	CachedTextures.Emplace(FImageUtils::ImportFileAsTexture2D(InFilePath));
}