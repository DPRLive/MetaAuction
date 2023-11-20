// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAChatRoomListWidget.generated.h"

/** UMAChatRoomListWidget
 * ChatRoomEntry의 리스트를 표시하는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAChatRoomListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatRoomListWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void Update();
	class UListView* GetListView() const;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UListView> ChatRoomListView;
};
