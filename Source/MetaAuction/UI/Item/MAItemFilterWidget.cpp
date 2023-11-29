// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Item/MAItemFilterWidget.h"

#include <GameFramework/GameState.h>
#include <Components/EditableTextBox.h>
#include <Components/Button.h>
#include <Components/ComboBoxString.h>

UMAItemFilterWidget::UMAItemFilterWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemFilterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchText->OnTextCommitted.AddDynamic(this, &ThisClass::SearchTextCommitted);
	SearchButton->OnClicked.AddDynamic(this, &ThisClass::SearchButtonClicked);

	ItemWorldTypeComboBox->ClearOptions();
	//MAGetNowWorldId(MAGetGameState())
	ItemWorldTypeComboBox->AddOption(TEXT("상관 없음"));
	ItemWorldTypeComboBox->AddOption(TEXT("웹에서만"));
	ItemWorldTypeComboBox->AddOption(TEXT("메타버스에서만"));
	ItemWorldTypeComboBox->SetSelectedIndex(0);
	
	// EItemDealType의 모든 항목의 DisplayName를 ComboBoxString의 옵션으로 설정합니다.
	ItemDealTypeComboBox->ClearOptions();
	for (uint8 EnumIndex = static_cast<uint8>(EItemDealType::None); EnumIndex < static_cast<uint8>(EItemDealType::MAX); ++EnumIndex)
	{
		FText DisplayName;
		const UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(TEXT("EItemDealType"));
		if (IsValid(EnumPtr))
		{
			DisplayName = EnumPtr->GetDisplayNameTextByValue(EnumIndex);
		}
		ItemDealTypeComboBox->AddOption(DisplayName.ToString());
	}
	ItemDealTypeComboBox->SetSelectedIndex(0);

	// EItemDealType의 모든 항목의 DisplayName를 ComboBoxString의 옵션으로 설정합니다.
	ItemCanDealComboBox->ClearOptions();
	for (uint8 EnumIndex = static_cast<uint8>(EItemCanDeal::None); EnumIndex < static_cast<uint8>(EItemCanDeal::MAX); ++EnumIndex)
	{
		FText DisplayName;
		const UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(TEXT("EItemCanDeal"));
		if (IsValid(EnumPtr))
		{
			DisplayName = EnumPtr->GetDisplayNameTextByValue(EnumIndex);
		}
		ItemCanDealComboBox->AddOption(DisplayName.ToString());
	}
	ItemCanDealComboBox->SetSelectedIndex(0);
}


void UMAItemFilterWidget::Search()
{
	OnSearch.Broadcast(GetCurrentOption());
}

FItemSearchOption UMAItemFilterWidget::GetCurrentOption()
{
	FItemSearchOption NewOption;
	NewOption.SearchString = SearchText->GetText().ToString();

	// 상관 없음
	if (ItemWorldTypeComboBox->GetSelectedIndex() == 0)
	{
		// NewOption.World를 설정하지 않음.
	}
	// 웹에서만
	else if (ItemWorldTypeComboBox->GetSelectedIndex() == 1)
	{
		NewOption.World = 0;
	}
	// 메타버스에서만
	else if (ItemWorldTypeComboBox->GetSelectedIndex() == 2)
	{
		NewOption.World = MAGetNowWorldId(MAGetGameState());
	}

	// enum의 0번째 옵션은 제거되었으므로 +1
	NewOption.ItemType = static_cast<EItemDealType>(ItemDealTypeComboBox->GetSelectedIndex() + 1);
	NewOption.CanDeal = static_cast<EItemCanDeal>(ItemCanDealComboBox->GetSelectedIndex() + 1);
	return NewOption;
}

void UMAItemFilterWidget::SearchButtonClicked()
{
	Search();
}

void UMAItemFilterWidget::SearchTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		Search();
	}
}