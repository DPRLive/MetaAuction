// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Actor/ItemActor.h"
#include "MAItemEntryWidget.generated.h"

struct FItemData;

/** UMAItemEntryWidget
 * Item의 정보를 표시하는 위젯으로 UMAItemListWidget의 리스트뷰의 엔트리 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAItemEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	UMAItemEntryWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:

	// 이 위젯을 FItemData 기반으로 모두 업데이트 합니다.
	void UpdateAll(const FItemData& InItemData);

protected:

	void UpdateText(const FItemData& InItemData);
	void UpdateImage(const FItemData& InItemData);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> BuyerNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> StartPriceText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> CurrentPriceText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> EndTimeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BineWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> DetailsButton;
};
