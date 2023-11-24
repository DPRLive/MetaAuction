// Fill out your copyright notice in the Description page of Project Settings.


#include "MACharacterDataSet.h"
#include "MACharacter.h"
#include "MACharacterControlData.h"
#include "MACharacterMeshData.h"
#include "Camera/MACameraModeData.h"
#include "Input/MAInputConfig.h"
#include "Input/MAInputInitializeComponent.h"

void UMACharacterDataSet::GiveToData(AMACharacter* InCharacter)
{
    if (InCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("[UMACharacterDataSet::GiveToData] InCharacter가 nullptr 입니다."));
        return;
    }

    if (CharacterControlData != nullptr)
    {
        CharacterControlData->GiveToData(InCharacter);
    }

    if (CharacterMeshData != nullptr)
    {
        CharacterMeshData->GiveToData(InCharacter);
    }

    if (CameraModeDatas.IsValidIndex(0) && IsValid(CameraModeDatas[0]))
    {
        CameraModeDatas[0]->GiveToData(InCharacter);
    }

    UMAInputInitializeComponent* InputInitializeComponent = UMAInputInitializeComponent::FindInputInitializeComponent(InCharacter);
    if (InputInitializeComponent != nullptr)
    {
        InputInitializeComponent->InputConfig = InputConfig;
        InputInitializeComponent->InputMapping = InputMapping;
    }
}
