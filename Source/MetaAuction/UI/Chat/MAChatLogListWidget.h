// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Queue.h"
#include "UI/Chat/MAChatLogEntry.h"
#include "MAChatLogListWidget.generated.h"

/** UMAChatLogListWidget
 * ChatLogEntry의 리스트를 표시하는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMAChatLogListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMAChatLogListWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable, Category = "ChatLog")
	void AddChatLog(const FMAChatLogEntryData& InData);

	UFUNCTION(BlueprintCallable, Category = "ChatLog")
	void ClearChatLog();

	UFUNCTION(BlueprintCallable, Category = "ChatLog")
	void ScrollChatLogToBottom();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UListView> ChatLogListView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxEntryCount;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentEntryCount;

	TQueue<UMAChatLogEntry*> Entries;
};
