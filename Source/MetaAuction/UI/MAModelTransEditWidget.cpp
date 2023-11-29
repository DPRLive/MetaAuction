// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAModelTransEditWidget.h"
#include "Player/MAPlayerController.h"

#include <Components/EditableTextBox.h>
#include <Components/Button.h>

UMAModelTransEditWidget::UMAModelTransEditWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ItemLoc = 0;
}

void UMAModelTransEditWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CancelBtn->OnClicked.AddDynamic(this, &UMAModelTransEditWidget::_OnClickCancelBtn);
	VerifyBtn->OnClicked.AddDynamic(this, &UMAModelTransEditWidget::_OnClickVerifyBtn);
}

/**
 *	UI에서 변경을 위해 사용할 Data를 넣습니다.
 *	@param InItemLoc : 변경을 시도할 물품의 레벨 상 위치
 *	@param InNowTransform : 현재 설정되어 있는 Transform
 */
void UMAModelTransEditWidget::PushData(const uint8 InItemLoc, const FTransform& InNowTransform)
{
	ItemLoc = InItemLoc;
	
	// 현재 Location 설정
	TransXInput->SetText(FText::AsNumber(InNowTransform.GetLocation().X));
	TransYInput->SetText(FText::AsNumber(InNowTransform.GetLocation().Y));
	TransZInput->SetText(FText::AsNumber(InNowTransform.GetLocation().Z));
	
	// 현재 Rotate 설정
	RotateRollInput->SetText(FText::AsNumber(InNowTransform.Rotator().Roll));
	RotatePitchInput->SetText(FText::AsNumber(InNowTransform.Rotator().Pitch));
	RotateYawInput->SetText(FText::AsNumber(InNowTransform.Rotator().Yaw));

	// 현재 Scale 설정
	ScaleInput->SetText(FText::AsNumber(InNowTransform.GetMinimumAxisScale()));
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
	// Roll의 경우 -180~180 사이 허용
	newRot.Roll = FMath::Clamp(_CastNum(RotateRollInput->GetText().ToString()), -179.0, 179.0);
	// Pitch의 경우 -90 ~ 90 까지 허용
	newRot.Pitch = FMath::Clamp(_CastNum(RotatePitchInput->GetText().ToString()), -89.0, 89.0);
	// Yaw의 경우 -180 ~ 180 허용
	newRot.Yaw = FMath::Clamp(_CastNum(RotateYawInput->GetText().ToString()), -179.0, 179.0);

	// scale 0으로 들어오면 1로 변환
	float scale = _CastNum(ScaleInput->GetText().ToString());
	scale = FMath::IsNearlyZero(scale) ? 1.f : scale;

	FTransform newTransform = FTransform(newRot, newLoc, FVector(scale));

	// 서버로 변경 요청
	if(AMAPlayerController* playerController = Cast<AMAPlayerController>(GetOwningPlayer()))
	{
		playerController->ServerRPC_SetModelRelativeTrans(MAGetMyJwtToken(GetGameInstance()), ItemLoc, newTransform);
	}
	
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
