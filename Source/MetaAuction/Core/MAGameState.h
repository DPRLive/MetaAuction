// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameStateBase.h>
#include "MAGameState.generated.h"

class UItemManager;
class UItemDataHandler;

/**
 * 
 */
UCLASS()
class METAAUCTION_API AMAGameState : public AGameStateBase
{
	GENERATED_BODY()

	AMAGameState();

public:
	virtual void PostInitializeComponents() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	
public:
	// ItemManager Getter
	FORCEINLINE TObjectPtr<UItemManager> GetItemManager() const { return ItemManager; }
	
	// ModelHandler Getter
	FORCEINLINE TObjectPtr<UItemDataHandler> GetItemDataHandler() const { return ItemDataHandler; }

	// World 정보 Getter
	FORCEINLINE const FString& GetWorldId() const { return WorldId; }
private:
	// 상품들을 관리하는 ItemManager
	UPROPERTY()
	TObjectPtr<UItemManager> ItemManager;

	// 아이템 관련 정보 처리를 위한 ItemDataHandler, RPC를 사용하기 때문에 Gameinstance가 아니라 여기에 두었습니다
	UPROPERTY()
	TObjectPtr<UItemDataHandler> ItemDataHandler;

	// 현재 접속된 world가 몇번째 world인지
	UPROPERTY( Replicated )
	FString WorldId;
};
