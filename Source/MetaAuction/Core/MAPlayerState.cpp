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
			ServerRPC_SendUserName(gameInstance->GetTempUserShareData().UserName);
			ServerRPC_SelectCharacter(gameInstance->GetTempUserShareData().SelectedCharacter);
		}
	}
}

void AMAPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMAPlayerState, UserData);
}

/**
* UserName을 서버로 보냅니다.
* @param InName : Client가 보낸 이름
*/
void AMAPlayerState::ServerRPC_SendUserName_Implementation(const FString& InName)
{
	LOG_N(TEXT("Receive User Name [%s]."), *InName);
	UserData.UserName = InName;
}

/**
* 선택한 캐릭터 Index를 서버로 보냅니다.
* Index는 DA_MeshInfo에 담기는 그 Index를 그대로 담아주세요.
* @param InIndex : Client가 선택한 캐릭터 Index
*/
void AMAPlayerState::ServerRPC_SelectCharacter_Implementation(const int16 InIndex)
{
	LOG_N(TEXT("Receive User Charcter Index [%d]."), InIndex);
	UserData.SelectedCharacter = InIndex;
}

/**
* UserData가 Replicate 되면 Delegate로 데이터가 들어왔다고 알립니다.
*/
void AMAPlayerState::OnRep_UserData() const
{
	if(OnReceiveUserData.IsBound())
		OnReceiveUserData.Broadcast();
}