// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MABidRecordListWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMABidRecordListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMABidRecordListWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void Update(const FItemData& InItemData);

private:

	int32 Order;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UListView> BidRecordListView;
};
