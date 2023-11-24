// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "Character/MACharacterData.h"
#include "MACameraModeData.generated.h"

/**
 *	카메라 모드 간 전환에 사용되는 블렌드 함수 입니다.
 */
UENUM(BlueprintType)
enum class EMACameraModeBlendFunction : uint8
{
	// 간단한 선형 보간을 수행합니다.
	Linear,

	// 즉시 가속하지만 타겟을 향해 부드럽게 감속합니다. 지수에 의해 제어되는 Ease 양입니다.
	EaseIn,

	// 부드럽게 가속하지만 대상으로 감속하지는 않습니다. 지수에 의해 제어되는 Ease 양입니다.
	EaseOut,

	// 부드럽게 가속 및 감속합니다. 지수에 의해 제어되는 Ease 양입니다.
	EaseInOut,

	// EMACameraModeBlendFunction 항목의 개수를 세기 위한 멤버.
	COUNT	UMETA(Hidden)
};

/**
 * 카메라 컴포넌트, 스프링암 컴포넌트를 설정하는 카메라 모드 데이터 애셋입니다.
 */
UCLASS()
class METAAUCTION_API UMACameraModeData : public UMACharacterData
{
	GENERATED_BODY()
	
public:

	UMACameraModeData();

	FORCEINLINE float GetBlendTime() const { return BlendTime; }

	/**
	* 해당 데이터 애셋의 값을 캐릭터에 적용합니다.
	*/
	virtual void GiveToData(class AMACharacter* InCharacter) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm)
	FVector RelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm)
	uint8 bSpringArmUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm, Meta = (EditCondition = "bSpringArmUsePawnControlRotation"))
	uint8 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm, Meta = (EditCondition = "bSpringArmUsePawnControlRotation"))
	uint8 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm, Meta = (EditCondition = "bSpringArmUsePawnControlRotation"))
	uint8 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm)
	uint8 bDoCollisionTest : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	uint8 bCameraUsePawnControlRotation : 1;

	// spring arm을 소켓에 부착 시, 어떤 이름의 소켓에 붙일지 입니다.
	// 이름이 설정되지 않으면(None) 부착하지 않습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpringArm)
	FName SocketName = NAME_None;
	
	// Blending Section
public:

	// 블렌딩을 사용하려면 체크하세요.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Blending)
	uint8 bUseBlending : 1;

	// 블렌딩에 사용되는 함수 입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Blending, Meta = (EditCondition = "bUseBlending"))
	EMACameraModeBlendFunction BlendFunction;

	// 블렌드 인 설정 입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Blending, Meta = (EditCondition = "bUseBlending"))
	float BlendTime;

	// 블렌딩 곡선의 모양을 제어하기 위해 블렌드 함수에서 사용하는 멱지수 입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Blending, Meta = (EditCondition = "bUseBlending"))
	float BlendExponent;
};
