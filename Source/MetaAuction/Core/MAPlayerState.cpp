// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MAPlayerState.h"
#include "MAGameInstance.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MAPlayerState)

void AMAPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	// 로컬 유저의 PlayerState 라면 game instance에 있는 나의 공유 데이터를 보낸다.
	if(!IsRunningDedicatedServer() && (GetPlayerController() != nullptr))
	{
		if(const UMAGameInstance* gameInstance = Cast<UMAGameInstance>(GetGameInstance()))
		{
			ServerRPC_SendUserData(gameInstance->GetTempUserShareData());
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
	LOG_N(TEXT("Receive [%s]User Data."), *InUserData.UserName);
	UserData = InUserData;
}

/**
* UserData가 Replicate 되면 Delegate로 데이터가 들어왔다고 알립니다.
*/
void AMAPlayerState::OnRep_UserData() const
{
	if(OnReceiveUserData.IsBound())
		OnReceiveUserData.Broadcast();
}