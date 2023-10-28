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
	// ItemManager Getter
	FORCEINLINE TObjectPtr<UItemManager> GetItemManager() const { return ItemManager; }
	
	// ModelHandler Getter
	FORCEINLINE TObjectPtr<UItemDataHandler> GetItemDataHandler() const { return ItemDataHandler; }
	
protected:
	virtual void BeginPlay() override;
	
private:
	// 상품들을 관리하는 ItemManager
	UPROPERTY()
	TObjectPtr<UItemManager> ItemManager;

	// 아이템 관련 정보 처리를 위한 ItemDataHandler, RPC를 사용하기 때문에 Gameinstance가 아니라 여기에 두었습니다
	UPROPERTY()
	TObjectPtr<UItemDataHandler> ItemDataHandler;
};
