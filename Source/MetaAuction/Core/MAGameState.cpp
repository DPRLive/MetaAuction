// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/MAGameState.h"
#include "../Manager/ItemManager.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameState)

AMAGameState::AMAGameState()
{
	ItemManager = CreateDefaultSubobject<UItemManager>(TEXT("ItemManager"));
	ItemDataHandler = CreateDefaultSubobject<UItemDataHandler>(TEXT("ItemDataHandler"));

	WorldId = TEXT("");
}

/**
 * 서버에서 월드를 설정합니다.
 */
void AMAGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsRunningDedicatedServer()) // 서버일 때만 월드 설정 // ServerTravel 시에도 잘 작동할지는 봐야함..
	{
		WorldId = DA_NETWORK(DediWorld);
	}
}

void AMAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMAGameState, WorldId);
}

void AMAGameState::BeginPlay()
{
	Super::BeginPlay();
}
