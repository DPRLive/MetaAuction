// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "MetaAuction.h"
#include "Components/ActorComponent.h"
#include "MAInputInitializeComponent.generated.h"

class UInputComponent;
class UInputInputConfig;
struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UMAInputInitializeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMAInputInitializeComponent();

	/** 액터로부터 UMAInputInitializeComponent를 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "MA")
	static UMAInputInitializeComponent* FindInputInitializeComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UMAInputInitializeComponent>() : nullptr); }

	// 실제 플레이어에 의해 제어되고 추가 입력 바인딩을 추가할 수 있는 초기화에서 충분히 진행된 경우 True입니다.
	FORCEINLINE bool IsReadyToBindInputs() const { return bReadyToBindInputs; };

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump(const FInputActionValue& Value);
	void Input_StopJumping(const FInputActionValue& Value);
	void Input_Interact(const FInputActionValue& Value);
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MA|Input")
	TObjectPtr<class UInputMappingContext> InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MA|Input")
	TObjectPtr<class UMAInputConfig> InputConfig;

protected:

	// 플레이어 입력 바인딩이 적용된 경우 true, 플레이어가 아닌 경우 false
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bReadyToBindInputs : 1;
};
