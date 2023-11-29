// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MAChatRoomEntryWidget.generated.h"

/** UMAChatRoomEntryWidget
 * ChatLog를 표시하는 위젯으로 UMAChatRoomListWidget의 리스트뷰의 엔트리 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAChatRoomEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UMAChatRoomEntryWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> SellerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> LastTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> LastMessageText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> ItemImage;

	FDelegateHandle OnChatDelegateHandle;
};
