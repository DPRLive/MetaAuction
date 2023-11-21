// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MAPlayerState.h"

#include <Net/UnrealNetwork.h>

#include "MAGameInstance.h"

void AMAPlayerState::BeginPlay()
{
	LOG_WARN(TEXT("Begin!"));
	Super::BeginPlay();
	LOG_WARN(TEXT("End!"));
	
	// 로컬 유저의 PlayerState 라면 game instance에 있는 나의 공유 데이터를 보낸다.
	if(!IsRunningDedicatedServer() && (GetPlayerController() != nullptr))
	{
		if(const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
		{
			LOG_WARN(TEXT("t2q"));
			ServerRPC_SendUserData(gameInstance->GetMyUserData());
		}
	}
}

void AMAPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMAPlayerState, UserData);
}

/**
* UserData를 Client가 보내준대로 설정합니다.
* @param InUserData : Client가 보낸 UserData
*/
void AMAPlayerState::ServerRPC_SendUserData_Implementation(const FUserShareData& InUserData)
{
	LOG_WARN(TEXT("sibal"));

	UserData = MoveTemp(UserData);
}

/**
* UserData가 Replicate 되면 Delegate로 변경이 있었다고 알립니다.
*/
void AMAPlayerState::OnRep_UserData() const
{
	if(OnChangeUserData.IsBound())
		OnChangeUserData.Broadcast();
}