// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MAChatLogEntryWidget.generated.h"

/** UMAChatLogEntryWidget
 * ChatLog를 표시하는 위젯으로 UMAChatLogListWidget의 리스트뷰의 엔트리 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAChatLogEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	UMAChatLogEntryWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> NameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> LogText;
};
