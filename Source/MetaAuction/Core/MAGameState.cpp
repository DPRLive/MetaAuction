// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/MAGameState.h"
#include "../Manager/ItemManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameState)

AMAGameState::AMAGameState()
{
	ItemManager = CreateDefaultSubobject<UItemManager>(TEXT("ItemManager"));
	ItemDataHandler = CreateDefaultSubobject<UItemDataHandler>(TEXT("ItemDataHandler"));
}

void AMAGameState::BeginPlay()
{
	Super::BeginPlay();
}
