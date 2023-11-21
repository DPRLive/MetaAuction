// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/UserShareData.h"

#include <GameFramework/PlayerState.h>
#include "MAPlayerState.generated.h"

/**
 *	 
 */
UCLASS()
class METAAUCTION_API AMAPlayerState : public APlayerState
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// UserData를 서버로 보냅니다.
	UFUNCTION(Server, Unreliable)
	void ServerRPC_SendUserData(const FUserShareData& InUserData);

private:
	UFUNCTION()
	void OnRep_UserData() const;

public:
	// UserData가 변경되면 변경되었다고 알려주는 Delegate 입니다.
	DECLARE_MULTICAST_DELEGATE(FOnChangeUserData)
	FOnChangeUserData OnChangeUserData;
	
private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UserData )
	FUserShareData UserData;
};
