// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation/AnimInstance.h>
#include "MAAnimInstance.generated.h"

class ACharacter;

/**
 * 캐릭터에 적용시킬 애니메이션 입니다.
 */
UCLASS()
class METAAUCTION_API UMAAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UMAAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	// 오우너
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<ACharacter> Owner;	
	
	// 이동 임계치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	float MovingThreshold;

	// 점프 임계치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	float JumpingThreshold;
	
	// 캐릭터의 우측 방향 속도
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float RSpeed;

	// 캐릭터의 앞쪽 방향 속도
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float FSpeed;

	// 가만히 있는지?
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsIdle : 1;
	
	// 캐릭터가 떨어지는 중인지? 
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsFalling : 1;

	// 점프 중인지?
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsJumping : 1;
};
