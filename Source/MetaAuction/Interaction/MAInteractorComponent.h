// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MAInteractorComponent.generated.h"


/** UMAInteractorComponent
 * 액터가 상호작용을 할 수 있도록 도와주는 컴포넌트 입니다. 사용하기 위해 아래와 같은 순서를 따라야 합니다.
 * step1. 상호작용을 하는 주체인 액터에 이 컴포넌트를 부착하세요. (ex. 플레이어 컨트롤러 or 플레이어 캐릭터 등)
 * step2. 상호작용을 하는 대상인 액터에 IMAInteractableInterface를 구현하세요.
 */
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UMAInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UMAInteractorComponent();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MA|Interact")
	float InteractingRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MA|Interact")
	TObjectPtr<class AActor> InteractingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MA|Interact")
	uint8 bDebug : 1;

private:

	void NotifyInteractingActorChanged(AActor* OldActor, AActor* NewActor);
	void UpdateInteracting();
};