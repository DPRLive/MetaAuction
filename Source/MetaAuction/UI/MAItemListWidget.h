// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Manager/ItemManager.h"
#include "MAItemListWidget.generated.h"

/** UMAItemListWidget
 * 아이템 정보의 리스트를 표시하는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAItemListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAItemListWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void UpdateSearchItems(const FItemSearchOption& InItemOption);
	void UpdateMyItems(EMyItemReqType InType);

private:

	
	void UpdateItems(const TArray<FItemData>& ItemData);
	UItemManager* GetItemManager();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UListView> ItemListView;
};
