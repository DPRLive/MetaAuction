// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAItemImageListWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAItemImageListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemImageListWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void Update(const FItemData& InItemData);
	class UMAItemImageEntry* GetSelectedItemImageEntry() const;
	class UListView* GetListView() const;

	UFUNCTION()
	void SelectPrevItem();

	UFUNCTION()
	void SelectNextItem();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UListView> ItemImageListView;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	int32 SelectedItemIndex;
};
