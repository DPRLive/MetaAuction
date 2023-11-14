// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MAInteractableInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMAInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/** IMAInteractableInterface
 * UMAInteractorComponent가 상호작용할 수 있도록 이 인터페이스를 액터에서 구현하세요.
 */
class METAAUCTION_API IMAInteractableInterface
{
	GENERATED_BODY()

public:

	// 상호작용할 수 있는지를 나타냅니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanInteracting() const;

	// 상호작용이 시작될 때 호출됩니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginInteracting(AActor* InteractorActor, FHitResult& HitResult);
	
	// 상호작용이 종료될 때 호출됩니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndInteracting(AActor* InteractorActor);
};
