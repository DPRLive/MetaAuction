// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatInfoWidget.h"
#include "UI/Chat/MAChatRoomEntry.h"
#include "UI/Chat/MAChatRoomListWidget.h"
#include "UI/Chat/MAChatRoomWidget.h"
#include "UI/MAAuctionWidget.h"
#include "Player/MAPlayerController.h"

#include "Components/ListView.h"

UMAChatInfoWidget::UMAChatInfoWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UMAChatInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WBP_ChatRoom->SetVisibility(ESlateVisibility::Hidden);
}

void UMAChatInfoWidget::NativeDestruct()
{
	GetListView()->OnItemSelectionChanged().Remove(OnItemSelectionChangedHandle);

	// MAAuctionWidget이 보이면 포커스 설정
	if (AMAPlayerController* MAPC = Cast<AMAPlayerController>(GetOwningPlayer()))
	{
		if (UMAAuctionWidget* MAAuctionWidget = MAPC->GetAuctionWidget())
		{
			if (MAAuctionWidget->GetVisibility() == ESlateVisibility::Visible)
			{
				MAAuctionWidget->SetFocus();
			}
		}
	}

	Super::NativeDestruct();
}

FReply UMAChatInfoWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		WBP_ChatRoom->SetFocusInputText();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMAChatInfoWidget::Update()
{
	WBP_ChatRoomList->Update();


	TWeakObjectPtr<ThisClass> ThisPtr = this;
	OnItemSelectionChangedHandle = GetListView()->OnItemSelectionChanged().AddLambda([ThisPtr](UObject* MyListView)
		{
			if (ThisPtr.IsValid())
			{
				if (UMAChatRoomEntry* Entry = Cast<UMAChatRoomEntry>(ThisPtr->GetListView()->GetSelectedItem()))
				{
					ThisPtr->WBP_ChatRoom->Update(Entry->Data);
					ThisPtr->WBP_ChatRoom->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ThisPtr->WBP_ChatRoom->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		});

}

void UMAChatInfoWidget::SelectListItem(const FChatRoomData& InData)
{
	UMAChatRoomEntry* Entry = NewObject<UMAChatRoomEntry>();
	Entry->Data = InData;
	GetListView()->SetSelectedItem(Entry);
}

UListView* UMAChatInfoWidget::GetListView() const
{
	return WBP_ChatRoomList->GetListView();
}
