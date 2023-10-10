// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "MACharacterData.h"
#include <Components/SkeletalMeshComponent.h>
#include "MACharacterMeshData.generated.h"

/**
 * 캐릭터의 메시 정보를 구성하는 데이터 애셋입니다.
 */
UCLASS()
class METAAUCTION_API UMACharacterMeshData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UMACharacterMeshData();

	/**
	* 해당 데이터 애셋의 값을 캐릭터에 적용합니다.
	*/
	void GiveToData(class AMACharacter* InCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class USkeletalMesh> CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	FCollisionProfileName CollisionProfileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	FTransform RelativeTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TSubclassOf<class UAnimInstance> AnimInstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TEnumAsByte<EAnimationMode::Type> AnimationMode;
};
