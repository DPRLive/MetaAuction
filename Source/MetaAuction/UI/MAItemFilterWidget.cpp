// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemFilterWidget.h"

#include <GameFramework/GameState.h>
#include <Components/EditableText.h>
#include <Components/Button.h>
#include <Components/ComboBoxString.h>

UMAItemFilterWidget::UMAItemFilterWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMAItemFilterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(SearchButton);
	if (IsValid(SearchButton))
	{
		SearchButton->OnClicked.AddDynamic(this, &ThisClass::SearchButtonClicked);
	}

	ensure(ItemDealTypeComboBox);
	if (IsValid(ItemDealTypeComboBox))
	{
		ItemDealTypeComboBox->ClearOptions();

		// EItemDealType의 모든 항목의 DisplayName를 ComboBoxString의 옵션으로 설정합니다.
		for (uint8 EnumIndex = static_cast<uint8>(EItemDealType::None); EnumIndex < static_cast<uint8>(EItemDealType::MAX); ++EnumIndex)
		{
			FString DisplayName;
			UEnum::GetValueAsString<EItemDealType>(static_cast<EItemDealType>(EnumIndex), DisplayName);
			ItemDealTypeComboBox->AddOption(DisplayName);
		}
	}

	ensure(ItemCanDealComboBox);
	if (IsValid(ItemCanDealComboBox))
	{
		ItemCanDealComboBox->ClearOptions();

		// EItemDealType의 모든 항목의 DisplayName를 ComboBoxString의 옵션으로 설정합니다.
		for (uint8 EnumIndex = static_cast<uint8>(EItemCanDeal::None); EnumIndex < static_cast<uint8>(EItemCanDeal::MAX); ++EnumIndex)
		{
			FString DisplayName;
			UEnum::GetValueAsString<EItemCanDeal>(static_cast<EItemCanDeal>(EnumIndex), DisplayName);
			ItemCanDealComboBox->AddOption(DisplayName);
		}
	}
}


void UMAItemFilterWidget::Search()
{
	if (!SearchText->GetText().IsEmpty())
	{
		OnSearch.Broadcast(GetCurrentOption());
	}
}

FItemSearchOption UMAItemFilterWidget::GetCurrentOption()
{
	FItemSearchOption NewOption;
	NewOption.SearchString = SearchText->GetText().ToString();
	// TODO: NewOption.World = ???
	NewOption.ItemType = static_cast<EItemDealType>(ItemDealTypeComboBox->GetSelectedIndex());
	NewOption.CanDeal = static_cast<EItemCanDeal>(ItemCanDealComboBox->GetSelectedIndex());
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