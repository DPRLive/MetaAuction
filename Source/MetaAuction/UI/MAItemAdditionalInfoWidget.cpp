﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAItemAdditionalInfoWidget.h"
#include "MetaAuction.h"

#include <Components/TextBlock.h>

UMAItemAdditionalInfoWidget::UMAItemAdditionalInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommentCount = 0;
}

void UMAItemAdditionalInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(TitleText);
	ensure(InformationText);
	ensure(SellerNameText);
}

void UMAItemAdditionalInfoWidget::Update(const FItemData& InItemData)
{
	CachedItemData = InItemData;
	TitleText->SetText(FText::FromString(InItemData.Title));
	InformationText->SetText(FText::FromString(InItemData.Information));
	SellerNameText->SetText(FText::FromString(InItemData.SellerName));

	UChatHandler* ChatHandler = MAGetChatHandler(MAGetGameInstance());
	if (!IsValid(ChatHandler))
	{
		return;
	}

	TWeakObjectPtr<ThisClass> ThisPtr(this);
	if (!ThisPtr.IsValid())
	{
		return;
	}
}