// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MACharacterPickerTileWidget.generated.h"

/**UMACharacterPickerTileWidget
 * UMACharacterPickerEntryWidget을 Entry로 사용하는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterPickerTileWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMACharacterPickerTileWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

public:

	void Update();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTileView> ItemTileView;
};
