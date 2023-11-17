// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "MAModelTransEditWidget.generated.h"

class AMAPlayerController;
class UEditableTextBox;
class UButton;

/**
 * 모델 트랜스폼을 변경해주는 UI입니다
 */
UCLASS()
class METAAUCTION_API UMAModelTransEditWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	virtual void RemoveFromParent() override;

private:
	UFUNCTION()
	void _OnClickVerifyBtn();

	UFUNCTION()
	void _OnClickCancelBtn();

	// string을 숫자로 캐스트합니다.
	float _CastNum(const FString& InString) const;
	
	// location 관련 text
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> TransXInput;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> TransYInput;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> TransZInput;

	// rotate 관련 text
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> RotateRollInput;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> RotatePitchInput;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> RotateYawInput;

	// scale 관련 text
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UEditableTextBox> ScaleInput;

	// 취소 버튼
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> CancelBtn;

	// 변경 확인 버튼
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> VerifyBtn;

	// 변경할 Item의 Location
	uint32 ItemLoc;

	// 변경을 요청할 컨트롤러
	TWeakObjectPtr<AMAPlayerController> RequestController; 
};
