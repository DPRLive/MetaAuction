// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MetaAuction.h"
#include "Components/ActorComponent.h"
#include "MACameraModeComponent.generated.h"

class UMACameraModeData;

/**
* UMACameraModeData의 블렌딩을 관리하는 컴포넌트 입니다.
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UMACameraModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UMACameraModeComponent();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* 카메라 모드를 바로 적용하기 위해 사용하는 함수입니다.
	*/
	UFUNCTION(BlueprintCallable)
	void SetCameraMode(UMACameraModeData* InCameraModeData);

	/**
	* 카메라 모드를 변경하기 위해 사용하는 함수입니다.
	*/
	UFUNCTION(BlueprintCallable)
	void PushCameraMode(UMACameraModeData* InCameraModeData);

private:

	/**
	* 시작 시 초기 블렌드 가중치를 결정합니다.
	*/
	void SetBlendWeight(float Weight);

	/**
	* 틱마다 블렌딩 변수를 업데이트 할 때 사용합니다.
	*/
	void UpdateBlending(float DeltaTime);

	/**
	* 블렌딩 알파를 적용합니다.
	*/
	void ApplyBlending();

public:

	UPROPERTY(EditAnywhere, Category = CameraMode)
	TObjectPtr<UMACameraModeData> CameraModeData;

private:

	UPROPERTY(VisibleAnywhere, Category = CameraMode, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMACameraModeData> NextCameraModeData;

	float StartTargetArmLength;
	FVector StartTargetArmLocation;

	// 블렌드 가중치를 결정하는 데 사용되는 선형 블렌드 알파입니다.
	float BlendAlpha;

	// 블렌드 알파와 함수를 사용하여 계산된 블렌드 가중치입니다.	
	float BlendWeight;
};
