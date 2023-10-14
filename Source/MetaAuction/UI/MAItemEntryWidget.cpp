// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemEntryWidget.h"
#include "UI/MAItemEntry.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include <ImageUtils.h>

UMAItemEntryWidget::UMAItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TitleText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TitleText")));
	ensure(TitleText);

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
}

void UMAItemEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UMAItemEntry* ItemEntry = Cast<UMAItemEntry>(ListItemObject);
	if (IsValid(ItemEntry))
	{
		UpdateAll(ItemEntry->ItemData);
	}
}

void UMAItemEntryWidget::UpdateAll(const FItemData& InItemData)
{
	UpdateText(InItemData);
	UpdateImage(InItemData);
}

void UMAItemEntryWidget::UpdateText(const FItemData& InItemData)
{
	TitleText->SetText(FText::FromString(InItemData.Title));
	BuyerNameText->SetText(FText::FromString(InItemData.BuyerName));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));

	// FNumberFormattingOptions ��ü ����
	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true); // ���� ������ ��� (ex. 1000000 -> 1,000,000

	StartPriceText->SetText(FText::AsNumber(InItemData.StartPrice, &NumberFormatOptions));
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));

	// �迭�� ��Ҹ� "."���� ���е� ���ڿ��� ��ġ��
	const int32 MaxEndTimeIndex = 5;
	const TArray<uint16>& EndTime = InItemData.EndTime;
	TArray<uint16> TempTime;
	TempTime.Init(0, 5);
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

void UMAItemEntryWidget::UpdateImage(const FItemData& InItemData)
{
	// ��ȿ�� �̹����� �߰ߵǸ� �� �̹����� ǥ���Ѵ�.
	// for (const FString& ImgPath : InItemData.ImgPaths)
	// {
	// 	UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(ImgPath);
	// 	if (IsValid(Texture))
	// 	{
	// 		ItemImage->SetBrushFromTexture(Texture, false);
	// 		break;
	// 	}
	// }
}