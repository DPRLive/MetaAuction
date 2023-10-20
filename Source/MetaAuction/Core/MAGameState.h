// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameStateBase.h>
#include "MAGameState.generated.h"

class UItemManager;
/**
 * 
 */
UCLASS()
class METAAUCTION_API AMAGameState : public AGameStateBase
{
	GENERATED_BODY()

	AMAGameState();

public:
	// ItemManager Getter
	FORCEINLINE TObjectPtr<UItemManager> GetItemManager() const { return ItemManager; }
	
protected:
	virtual void BeginPlay() override;
	
private:
	// 상품들을 관리하는 ItemManager
	UPROPERTY()
	TObjectPtr<UItemManager> ItemManager;
};
