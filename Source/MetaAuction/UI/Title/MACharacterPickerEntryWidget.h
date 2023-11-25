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

	// UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	// ~UUserWidget interface

	// IUserObjectListEntry interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	// ~IUserObjectListEntry interface

private:

	void Hover();
	void Unhover();
	void Select();
	void Unselect();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UImage> CharacterImage;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Appearance", meta = (BindWidget, AllowPrivateAccess = "true"))
	FLinearColor NormalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (BindWidget, AllowPrivateAccess = "true"))
	FLinearColor HoveredColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (BindWidget, AllowPrivateAccess = "true"))
	FLinearColor SelectedColor;
};
