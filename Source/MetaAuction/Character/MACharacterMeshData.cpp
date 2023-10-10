// Fill out your copyright notice in the Description page of Project Settings.


#include "MACharacterMeshData.h"
#include "MACharacter.h"

#include <Animation/AnimInstance.h>

UMACharacterMeshData::UMACharacterMeshData()
{
    CollisionProfileName.Name = TEXT("CharacterMesh");

    RelativeTransform.SetLocation(FVector(0.0f, 0.0f, -100.0f));
    RelativeTransform.SetRotation(FRotator(0.0f, -90.0f, 0.0f).Quaternion());
    RelativeTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

    AnimationMode = EAnimationMode::AnimationBlueprint;
}

void UMACharacterMeshData::GiveToData(AMACharacter* InCharacter)
{
    if (InCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("[UMACharacterControlData::GiveToData] InCharacter가 nullptr 입니다."));
        return;
    }

    // Mesh
    InCharacter->GetMesh()->SetRelativeTransform(RelativeTransform);
    InCharacter->GetMesh()->SetAnimationMode(AnimationMode);
    InCharacter->GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
    InCharacter->GetMesh()->SetSkeletalMesh(CharacterMesh);
    InCharacter->GetMesh()->SetAnimInstanceClass(AnimInstanceClass);
}
