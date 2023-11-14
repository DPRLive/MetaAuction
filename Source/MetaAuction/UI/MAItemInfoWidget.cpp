// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemInfoWidget.h"
#include "UI/MAItemImageEntry.h"
#include "UI/MAItemImageListWidget.h"
#include "UI/MACommentWidget.h"
#include "UI/MAItemAdditionalInfoWidget.h"
#include "UI/MAConfirmCancelPopupWidget.h"
#include "UI/MAConfirmPopupWidget.h"
#include "UI/MAAuctionWidget.h"
#include "UI/MAItemFilterWidget.h"
#include "Player/MAPlayerController.h"
#include "Common/MALog.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/ListView.h>
#include <Components/EditableTextBox.h>
#include <ImageUtils.h>

UMAItemInfoWidget::UMAItemInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BidMinimum = 100;
}

void UMAItemInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(ItemImage);
	ensure(TitleText);
	ensure(InformationText);
	ensure(BuyerNameText);
	ensure(SellerNameText);
	ensure(StartPriceText);
	ensure(CurrentPriceText);
	ensure(EndTimeText);
	ensure(ItemImagePrevButton);
	ensure(ItemImageNextButton);
	ensure(WBP_ItemImageList);
	ensure(BidPriceText);
	ensure(BidButton);
	ensure(ChatButton);
	ensure(DeleteButton);
	ensure(DetailsButton);
	ensure(WBP_Comment);

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

	if (IsValid(BidPriceText))
	{
		BidPriceText->SetVisibility(ESlateVisibility::Collapsed);
		BidPriceText->OnTextChanged.AddDynamic(this, &ThisClass::BidPriceTextChanged);
		BidPriceText->OnTextCommitted.AddDynamic(this, &ThisClass::BidPriceTextCommited);
	}

	if (IsValid(BidButton))
	{
		BidButton->SetVisibility(ESlateVisibility::Collapsed);
		BidButton->OnClicked.AddDynamic(this, &ThisClass::BidButtonClicked);
	}

	if (IsValid(ChatButton))
	{
		ChatButton->SetVisibility(ESlateVisibility::Collapsed);
		ChatButton->OnClicked.AddDynamic(this, &ThisClass::ChatButtonClicked);
	}

	if (IsValid(DeleteButton))
	{
		DeleteButton->SetVisibility(ESlateVisibility::Collapsed);
		DeleteButton->OnClicked.AddDynamic(this, &ThisClass::DeleteButtonClicked);
	}

	if (IsValid(DetailsButton))
	{
		DetailsButton->OnClicked.AddDynamic(this, &ThisClass::DetailsButtonClicked);
	}
}

void UMAItemInfoWidget::NativeDestruct()
{
	if (UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState()))
	{
		ItemDataHandler->OnChangePrice.Remove(OnChangePriceHandle);
	}

	if (IsValid(WBP_ItemImageList))
	{
		WBP_ItemImageList->GetListView()->OnItemSelectionChanged().Remove(ItemImageListViewSelectionChangedHandle);
	}

	Super::NativeDestruct();
}

void UMAItemInfoWidget::Update(const FItemData& InItemData)
{
	CachedItemData = InItemData;

	TitleText->SetText(FText::FromString(InItemData.Title));
	InformationText->SetText(FText::FromString(InItemData.Information));
	BuyerNameText->SetText(FText::FromString(InItemData.BuyerName));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.SetUseGrouping(true);

	StartPriceText->SetText(FText::AsNumber(InItemData.StartPrice, &NumberFormatOptions));
	CurrentPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice, &NumberFormatOptions));

	NumberFormatOptions.SetUseGrouping(false);
	BidPriceText->SetText(FText::AsNumber(InItemData.CurrentPrice + BidMinimum, &NumberFormatOptions));

	const int32 MaxEndTimeIndex = 5;

	WBP_ItemImageList->Update(InItemData);
	UMAItemImageEntry* Entry = WBP_ItemImageList->GetSelectedItemImageEntry();
	if (IsValid(Entry))
	{
		ItemImage->SetBrushFromTextureDynamic(Entry->Data.Image);
	}

	if (IsValid(WBP_Comment))
	{
		WBP_Comment->Update(InItemData);
	}

	if (AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer()))
	{
		if (UMAAuctionWidget* AuctionWidget = MAPC->GetAuctionWidget())
		{
			EItemCanDeal ItemCanDeal = AuctionWidget->GetCachedItemCanDeal();
			if (ItemCanDeal == EItemCanDeal::Possible)
			{
				// 입찰 버튼 보이기 (판매중 && 경매 && 판매자가 내가 아닌경우) 
				if (IsValid(BidButton) && CachedItemData.Type == EItemDealType::Auction && MAGetMyUserName(MAGetGameInstance()) != CachedItemData.SellerName)
				{
					if (IsValid(BidPriceText))
					{
						BidPriceText->SetVisibility(ESlateVisibility::Visible);
					}
					BidButton->SetVisibility(ESlateVisibility::Visible);
				}

				// 채팅하기 버튼 보이기 (판매중 && 일반 판매 && 판매자가 내가 아닌경우)
				if (IsValid(ChatButton) && CachedItemData.Type == EItemDealType::Normal && MAGetMyUserName(MAGetGameInstance()) != CachedItemData.SellerName)
				{
					ChatButton->SetVisibility(ESlateVisibility::Visible);
				}

				// 삭제하기 버튼 보이기 (판매중 && 내 물품인 경우)
				if (IsValid(DeleteButton) && MAGetMyUserName(MAGetGameInstance()) == CachedItemData.SellerName)
				{
					DeleteButton->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState()))
	{
		// TODO: 현재 OnChangePrice가 작동하지 않습니다!
		ItemDataHandler->OnChangePrice.Remove(OnChangePriceHandle);
		OnChangePriceHandle = ItemDataHandler->OnChangePrice.AddLambda([ThisPtr](const uint32 ItemID, const uint64 Price)
			{
				LOG_WARN(TEXT("입찰가 변동 ID : %d"), ItemID);
				if (ThisPtr.IsValid() && ThisPtr->CachedItemData.ItemID == ItemID)
				{
					LOG_WARN(TEXT("입찰가 변동 ID : %d"), Price);
					FNumberFormattingOptions NumberFormatOptions;
					NumberFormatOptions.SetUseGrouping(true);
					ThisPtr->CachedItemData.CurrentPrice = Price;
					ThisPtr->CurrentPriceText->SetText(FText::AsNumber(Price, &NumberFormatOptions));
					NumberFormatOptions.SetUseGrouping(false);
					ThisPtr->BidPriceText->SetText(FText::AsNumber(Price + ThisPtr->BidMinimum));
				}
			});
		if (OnChangePriceHandle.IsValid())
		{
			LOG_WARN(TEXT("입찰가 변동 델리게이트 등록 성공"));
		}
	}
}

void UMAItemInfoWidget::ItemImagePrevButtonClicked()
{
	if (IsValid(WBP_ItemImageList) && CachedItemData.ImgCount > 1)
	{
		WBP_ItemImageList->SelectPrevItem();
	}
}

void UMAItemInfoWidget::ItemImageNextButtonClicked()
{
	if (IsValid(WBP_ItemImageList) && CachedItemData.ImgCount > 1)
	{
		WBP_ItemImageList->SelectNextItem();
	}
}

void UMAItemInfoWidget::BidButtonClicked()
{
	if (AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer()))
	{
		if (UMAConfirmCancelPopupWidget* PopupWidget = MAPC->CreateAndAddConfirmCancelPopupWidget())
		{
			PopupWidget->SetText(TEXT("입찰하시겠습니까?"));
			TWeakObjectPtr<ThisClass> ThisPtr(this);
			auto RequestPopupTypeFunc = [ThisPtr](EMAConfirmCancelPopupType Type)
				{
					if (AMAPlayerController* MAPC = Cast<AMAPlayerController>(ThisPtr->GetOwningPlayer()))
					{
						UMAConfirmPopupWidget* PopupWidget = MAPC->CreateAndAddConfirmPopupWidget();
						if (ThisPtr.IsValid() && Type == EMAConfirmCancelPopupType::Confirm)
						{
							UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState());
							if (IsValid(ItemDataHandler))
							{
								int32 Price = FCString::Atoi(*ThisPtr->BidPriceText->GetText().ToString());
								LOG_WARN(TEXT("입찰가 : %d"), Price);
								ItemDataHandler->Client_RequestBid(ThisPtr->CachedItemData.ItemID, Price);
							}
							if (IsValid(PopupWidget))
							{
								PopupWidget->SetText(TEXT("입찰을 완료하였습니다."));
							}
						}
						else
						{
							if (IsValid(PopupWidget))
							{
								PopupWidget->SetText(TEXT("입찰을 실패하였습니다."));
							}
						}
					}
				};
			PopupWidget->OnDetermined.AddLambda(RequestPopupTypeFunc);
		}
	}
}

void UMAItemInfoWidget::ChatButtonClicked()
{

}

void UMAItemInfoWidget::DeleteButtonClicked()
{

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

// 텍스트 상자의 OnTextChanged 이벤트 핸들러에서 호출되는 함수
void UMAItemInfoWidget::BidPriceTextChanged(const FText& InText)
{
	if (IsValid(BidPriceText))
	{
		FString FilteredString = "";
		FString InString = InText.ToString();

		for (int32 i = 0; i < InString.Len(); ++i)
		{
			// 숫자인 경우에만 문자열에 추가
			if (FChar::IsDigit(InString[i]))
			{
				FilteredString.AppendChar(InString[i]);
			}
		}

		BidPriceText->SetText(FText::FromString(FilteredString));
	}
}

void UMAItemInfoWidget::BidPriceTextCommited(const FText& InText, ETextCommit::Type InCommitMethod)
{
	int32 Price = FCString::Atoi(*InText.ToString());
	if (Price < CachedItemData.CurrentPrice + BidMinimum)
	{
		BidPriceText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CachedItemData.CurrentPrice + BidMinimum)));
	}
}
