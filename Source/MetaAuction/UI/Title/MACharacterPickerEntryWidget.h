// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MACharacterPickerEntryWidget.generated.h"

/**UMACharacterPickerEntryWidget
 * UMACharacterPickerEntry을 Entry로 사용하는 위젯입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterPickerEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	UMACharacterPickerEntryWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> CharacterImage;
};
