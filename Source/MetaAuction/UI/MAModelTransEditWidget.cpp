// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAModelTransEditWidget.h"

#include <Components/EditableTextBox.h>
#include <Components/Button.h>

void UMAModelTransEditWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensure(CancelBtn);
	ensure(VerifyBtn);
	ensure(TransXInput);
	ensure(TransYInput);
	ensure(TransZInput);
	ensure(RotateRollInput);
	ensure(RotatePitchInput);
	ensure(RotateYawInput);
	ensure(ScaleInput);

	CancelBtn->OnClicked.AddDynamic(this, &UMAModelTransEditWidget::_OnClickCancelBtn);
	VerifyBtn->OnClicked.AddDynamic(this, &UMAModelTransEditWidget::UMAModelTransEditWidget::_OnClickVerifyBtn);

	ItemLoc = 0;
	RequestController = nullptr;
}

/**
 *	데이터를 정리하고 닫습니다.
 */
void UMAModelTransEditWidget::RemoveFromParent()
{
	ItemLoc = 0;
	RequestController = nullptr;
	
	Super::RemoveFromParent();
}

/**
 *	변경 버튼을 누르면 호출, 필요한 정보를 PlayerController의 Server RPC를 통해 서버로 보냅니다.
 */
void UMAModelTransEditWidget::_OnClickVerifyBtn()
{
	if(ItemLoc == 0)
	{
		RemoveFromParent();
		return;
	}
	
	FVector newLoc;
	newLoc.X = _CastNum(TransXInput->GetText().ToString());
	newLoc.Y = _CastNum(TransYInput->GetText().ToString());
	newLoc.Z = _CastNum(TransZInput->GetText().ToString());

	FRotator newRot;
	newRot.Roll = _CastNum(RotateRollInput->GetText().ToString());
	newRot.Pitch = _CastNum(RotatePitchInput->GetText().ToString());
	newRot.Yaw = _CastNum(RotateYawInput->GetText().ToString());

	// scale 0으로 들어오면 1로 변환
	float scale = _CastNum(ScaleInput->GetText().ToString());
	scale = FMath::IsNearlyZero(0.f) ? 1.f : scale;

	FTransform newTransform = FTransform(newRot, newLoc, FVector(scale));

	// TODO : PC받아와서 요청해야하
	
	
	RemoveFromParent();
}

/**
 *	취소 버튼을 누르면 호출합니다.
 */
void UMAModelTransEditWidget::_OnClickCancelBtn()
{
	RemoveFromParent();
}

/**
 *	string을 숫자로 캐스트합니다.
 *	숫자 형태가 아닌 경우 0을 반환합니다.
 */
float UMAModelTransEditWidget::_CastNum(const FString& InString) const
{
	return InString.IsNumeric() ? FCString::Atof(*InString) : 0.f; 
}
