// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "MACharacterData.h"
#include "MACharacterControlData.generated.h"

/**
 * 캐릭터의 컨트롤러의 방향, 무브먼트를 설정하는 데이터 애셋입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterControlData : public UMACharacterData
{
	GENERATED_BODY()
	
public:

	UMACharacterControlData();

	virtual void GiveToData(class AMACharacter* InCharacter) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	uint8 bUseControllerRotationPitch : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	uint8 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	uint8 bUseControllerRotationRoll : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterMovement)
	uint8 bOrientRotationToMovement : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterMovement)
	uint8 bUseControllerDesiredRotation : 1;

	/** Initial velocity (instantaneous vertical acceleration) when jumping. */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float JumpZVelocity;

	/**
	* When falling, amount of lateral movement control available to the character.
	* 0 = no control, 1 = full control at max speed of MaxWalkSpeed.
	*/
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float AirControl;

	/** Change in rotation per second, used when UseControllerDesiredRotation or OrientRotationToMovement are true. Set a negative value for infinite rotation rate and instant turns. */
	UPROPERTY(Category = "Character Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	FRotator RotationRate;

	/** The maximum ground speed when walking. Also determines maximum lateral speed when falling. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeed;

	/** The ground speed that we should accelerate up to when walking at minimum analog stick tilt */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MinAnalogWalkSpeed;

	/**
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float BrakingDecelerationWalking;
};
