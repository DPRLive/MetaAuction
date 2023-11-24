// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/UserShareData.h"

#include <GameFramework/PlayerState.h>
#include "MAPlayerState.generated.h"

/**
 *	Player State 입니다
 */
UCLASS()
class METAAUCTION_API AMAPlayerState : public APlayerState
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// UserData를 서버로 보냅니다.
	UFUNCTION(Server, Reliable)
	void ServerRPC_SendUserData(const FUserShareData& InUserData);

	// User Data Getter
	FORCEINLINE const FUserShareData& GetUserData() const { return UserData; }
	
private:
	UFUNCTION()
	void OnRep_UserData() const;

public:
	// UserData가 수신 되었다고 알려주는 Delegate 입니다.
	DECLARE_MULTICAST_DELEGATE(FOnReceiveUserData)
	FOnReceiveUserData OnReceiveUserData;
	
private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UserData )
	FUserShareData UserData;
};
