// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MABidRecordListWidget.h"
#include "UI/MABidRecordEntry.h"
#include "Handler/ItemDataHandler.h"

#include <Components/ListView.h>

UMABidRecordListWidget::UMABidRecordListWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UMABidRecordListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(BidRecordListView);
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
						for (const FBidRecord& BidRecord : InBidRecords)
						{
							UMABidRecordEntry* Entry = NewObject<UMABidRecordEntry>();
							Entry->Data = BidRecord;
							ThisPtr->BidRecordListView->AddItem(Entry);
						}
					}
				};
			ItemDataHandler->RequestBidRecordByItemId(Func, InItemData.ItemID);
		}
	}
}
