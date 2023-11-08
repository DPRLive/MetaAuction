// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAItemTileWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAItemTileWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemTileWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void UpdateSearchItems(const FItemSearchOption& InItemOption);
	void UpdateMyItems(EMyItemReqType InType);

private:

	void UpdateItems(const TArray<FItemData>& ItemData);

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTileView> ItemTileView;
};
