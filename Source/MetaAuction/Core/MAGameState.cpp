// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/MAGameState.h"
#include "../Manager/ItemManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAGameState)

AMAGameState::AMAGameState()
{
	ItemManager = CreateDefaultSubobject<UItemManager>(TEXT("ItemManager"));
}

void AMAGameState::BeginPlay()
{
	Super::BeginPlay();
	
	// 데디 서버를 키기 전 웹서버에 등록되어 있던 아이템들의 정보를 모두 가져온다.
	if(IsRunningDedicatedServer())
	{
		ItemManager->Server_RegisterAllWorldItemID();
	}
}
