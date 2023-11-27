// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BidRecord/MABidRecordListWidget.h"
#include "UI/BidRecord/MABidRecordEntry.h"
#include "Handler/ItemDataHandler.h"

#include <Components/ListView.h>

UMABidRecordListWidget::UMABidRecordListWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Order = 0;
}

void UMABidRecordListWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMABidRecordListWidget::Update(const FItemData& InItemData)
{
	BidRecordListView->ClearListItems();

	UItemDataHandler* ItemDataHandler = MAGetItemDataHandler(MAGetGameState());
	if (nullptr != ItemDataHandler)
	{
		for (int32 i = 1; i <= InItemData.ImgCount; i++)
		{
			TWeakObjectPtr<ThisClass> ThisPtr(this);
			auto Func = [ThisPtr](const TArray<FBidRecord>& InBidRecords)
				{
					if (ThisPtr.IsValid())
					{
						ThisPtr->Order = InBidRecords.Num();
						for (const FBidRecord& BidRecord : InBidRecords)
						{
							// Entry->Data.Order는 기본키 값이므로 순서가 이상할 수 있으므로 직접 세야 한다.
							UMABidRecordEntry* Entry = NewObject<UMABidRecordEntry>();
							Entry->Data = BidRecord;
							Entry->Data.Order = ThisPtr->Order--;
							ThisPtr->BidRecordListView->AddItem(Entry);
						}
					}
				};
			ItemDataHandler->RequestBidRecordByItemId(Func, InItemData.ItemID);
		}
	}
}
