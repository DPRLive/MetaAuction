// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MAChatInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class METAAUCTION_API UMAChatInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatInfoWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	void Update();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UMAChatRoomListWidget> WBP_ChatRoomList;
};