// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Item/MAItemEntryWidget.h"
#include "UI/Item/MAItemEntry.h"
#include "UI/ItemInfo/MAItemInfoWidget.h"
#include "UI/MAWidgetHelperLibrary.h"
#include "Handler/ItemFileHandler.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include <Components/HorizontalBox.h>
#include <ImageUtils.h>

UMAItemEntryWidget::UMAItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(SellerNameText);
	ensure(CurrentPriceText);
	ensure(BuyerNameBox);
	ensure(BuyerNameText);
	ensure(EndTimeText);
	ensure(ItemImage);
	ensure(DetailsButton);
	ensure(ItemInfoWidgetClass);

	if (IsValid(DetailsButton))
	{
		DetailsButton->OnClicked.AddDynamic(this, &ThisClass::DetailsButtonClicked);
	}
}

void UMAItemEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState(GetWorld()));
	if (IsValid(ItemDataHandler))
	{
		ItemDataHandler->OnChangeItemData.Remove(OnChangeItemDataHandle);
	}
}

void UMAItemEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (UMAItemEntry* ItemEntry = Cast<UMAItemEntry>(ListItemObject))
	{
		UpdateAll(ItemEntry->ItemData);

		UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState(GetWorld()));
		if (!IsValid(ItemDataHandler))
		{
			return;
		}

		TWeakObjectPtr<ThisClass> ThisPtr(this);
		auto OnChangeItemData = [ThisPtr](const uint32& InItemId, const FString& InWorld, const FString& InChangedData)
			{
				if (ThisPtr.IsValid())
				{
					UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState(ThisPtr->GetWorld()));
					if (!IsValid(ItemDataHandler))
					{
						return;
					}

					auto RequestItemDataByIdFunc = [ThisPtr](const FItemData& InData)
						{
							if (ThisPtr.IsValid())
							{
								if (UMAItemEntry* ItemEntry = Cast<UMAItemEntry>(ThisPtr->GetListItem()))
								{
									ItemEntry->ItemData = InData;
									ThisPtr->UpdateAll(InData);
								}
							}
						};
					ItemDataHandler->RequestItemDataById(RequestItemDataByIdFunc, InItemId);
				}
			};
		ItemDataHandler->OnChangeItemData.Remove(OnChangeItemDataHandle);
		OnChangeItemDataHandle = ItemDataHandler->OnChangeItemData.AddLambda(OnChangeItemData);
	}
}

void UMAItemEntryWidget::NativeOnEntryReleased()
{
}

void UMAItemEntryWidget::UpdateAll(const FItemData& InItemData)
{
	CachedItemData = InItemData;

	UpdateText(InItemData);
	UpdateImage(InItemData);
}

void UMAItemEntryWidget::UpdateText(const FItemData& InItemData)
{
	TitleText->SetText(FText::FromString(InItemData.Title));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));
	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true);
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));
	BuyerNameText->SetText(FText::FromString(InItemData.BuyerName.IsEmpty() ? TEXT("없음") : InItemData.BuyerName));
	EndTimeText->SetText(FText::FromString(InItemData.EndTime.ToString()));

	// 경매가 아니면 숨기기
	if (InItemData.Type != EItemDealType::Auction)
	{
		BuyerNameBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMAItemEntryWidget::UpdateImage(const FItemData& InItemData)
{
	//FItemFileHandler* ItemFileHandler = MAGetItemFileHandler(MAGetGameInstance(GetWorld()));
	//if (nullptr != ItemFileHandler)
	//{
	//	TWeakObjectPtr<ThisClass> ThisPtr(this);
	//	if (ThisPtr.IsValid())
	//	{
	//		auto Func = [ThisPtr](UTexture2DDynamic* InImage)
	//			{
	//				if (ThisPtr.IsValid())
	//				{
	//					ThisPtr->ItemImage->SetBrushFromTextureDynamic(InImage);
	//				}
	//			};
	//
	//		// 0번 인덱스 이미지는 없으므로 1번 인덱스 이미지를 가져옵니다.
	//		ItemFileHandler->RequestImg(Func, InItemData.ItemID, 1);
	//	}
	//}

	UMAWidgetHelperLibrary::RequestImageByItemID(ItemImage, InItemData.ItemID);
}

void UMAItemEntryWidget::DetailsButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		return;
	}

	UMAItemEntry* ItemEntry = Cast<UMAItemEntry>(GetListItem());
	if (IsValid(ItemEntry))
	{
		UpdateAll(ItemEntry->ItemData);
	}

	UMAItemInfoWidget* ItemInfoWidget = CreateWidget<UMAItemInfoWidget>(PC, ItemInfoWidgetClass);
	if (IsValid(ItemInfoWidget))
	{
		ItemInfoWidget->AddToViewport();
		ItemInfoWidget->Update(ItemEntry->ItemData);
	}
}
