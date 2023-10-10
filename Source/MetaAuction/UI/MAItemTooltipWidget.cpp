// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemTooltipWidget.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>
#include <ImageUtils.h>

UMAItemTooltipWidget::UMAItemTooltipWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentImageIndex = 0;
}

void UMAItemTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TitleText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TitleText")));
	ensure(TitleText);

	InformationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InformationText")));
	ensure(InformationText);

	BuyerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BuyerNameText")));
	ensure(BuyerNameText);

	SellerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("SellerNameText")));
	ensure(SellerNameText);

	StartPriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StartPriceText")));
	ensure(StartPriceText);

	CurrentPriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrentPriceText")));
	ensure(CurrentPriceText);

	EndTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("EndTimeText")));
	ensure(EndTimeText);

	ItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemImage")));
	ensure(ItemImage);

	ItemImagePrevButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemImagePrevButton")));
	ensure(ItemImagePrevButton);
	if (IsValid(ItemImagePrevButton))
	{
		ItemImagePrevButton->OnClicked.AddDynamic(this, &ThisClass::ItemImagePrevButtonClicked);
	}

	ItemImageNextButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemImageNextButton")));
	ensure(ItemImageNextButton);
	if (IsValid(ItemImageNextButton))
	{
		ItemImageNextButton->OnClicked.AddDynamic(this, &ThisClass::ItemImageNextButtonClicked);
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

	// FNumberFormattingOptions 객체 생성
	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true); // 숫자 구분자 사용 (ex. 1000000 -> 1,000,000

	StartPriceText->SetText(FText::AsNumber(InItemData.StartPrice, &NumberFormatOptions));
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));

	// 배열의 요소를 "."으로 구분된 문자열로 합치기
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
	else
	{
		// 이미지 파일이 존재하지 않습니다.
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
	CachedTextures.Empty();
	for (const FString& ImgPath : InItemData.ImgPaths)
	{
		AddImage(ImgPath);
	}
}

void UMAItemTooltipWidget::AddImage(const FString& InFilePath)
{
	// 외부 이미지를 UTexture2D로 임포트
	CachedTextures.Emplace(FImageUtils::ImportFileAsTexture2D(InFilePath));
}