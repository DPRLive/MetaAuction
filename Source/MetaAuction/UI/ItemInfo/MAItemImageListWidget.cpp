// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/MAItemImageListWidget.h"
#include "UI/ItemInfo/MAItemImageEntry.h"
#include "Common/MALog.h"

#include <Components/Button.h>
#include <Components/ListView.h>
#include <Engine/Texture2DDynamic.h>

UMAItemImageListWidget::UMAItemImageListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMAItemImageListWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMAItemImageListWidget::Update(const FItemData& InItemData)
{
	ItemImageListView->ClearListItems();

	if (FItemFileHandler* ItemFileHandler = MAGetItemFileHandler(MAGetGameInstance(GetWorld())))
	{
		for (int32 i = 1; i <= InItemData.ImgCount; i++)
		{
			TWeakObjectPtr<ThisClass> ThisPtr(this);
			auto Func = [ThisPtr](UTexture2DDynamic* InImage)
				{
					if (ThisPtr.IsValid())
					{
						UMAItemImageEntry* ItemImageEntry = NewObject<UMAItemImageEntry>();
						ItemImageEntry->Data.Image = InImage;
						ThisPtr->ItemImageListView->AddItem(ItemImageEntry);

						if (ThisPtr->ItemImageListView->GetNumItems() == 1)
						{
							ThisPtr->SelectedItemIndex = 0;
							ThisPtr->ItemImageListView->SetSelectedIndex(ThisPtr->SelectedItemIndex);
						}
					}
				};
			ItemFileHandler->RequestImg(Func, InItemData.ItemID, i);
		}
	}
}

UMAItemImageEntry* UMAItemImageListWidget::GetSelectedItemImageEntry() const
{
	return Cast<UMAItemImageEntry>(ItemImageListView->GetSelectedItem());
}

UListView* UMAItemImageListWidget::GetListView() const
{
	return ItemImageListView;
}

void UMAItemImageListWidget::SelectPrevItem()
{
	if (SelectedItemIndex > 0)
	{
		SelectedItemIndex--;
		ItemImageListView->SetSelectedIndex(SelectedItemIndex);
	}
}

void UMAItemImageListWidget::SelectNextItem()
{
	if (SelectedItemIndex < ItemImageListView->GetNumItems() - 1)
	{
		SelectedItemIndex++;
		ItemImageListView->SetSelectedIndex(SelectedItemIndex);
	}
}