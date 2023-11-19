// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "glTFRuntimeFunctionLibrary.h"

#include <Components/ActorComponent.h>
#include "ModelDrawComponent.generated.h"

class AglTFRuntimeAssetActor;

/**
 * 모델링 파일을 받아와 그려주는 (레벨에 배치해주는) Component 입니다.
 * 한개의 모델만 담당합니다.
 * 필요가 없어지면 꼭! 모델링을 지우는 함수를 호출해주세요.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UModelDrawComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UModelDrawComponent();

	virtual void InitializeComponent() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// 모델 파일 경로를 기반으로 모델을 만들어 배치합니다.
	void CreateModel(const FString& InGlbPath, const FTransform& InWorldTransform);

	// item id를 기반으로 모델을 만들어 배치합니다.
	void CreateModel(const uint32 InItemId, const FTransform& InWorldTransform);
	
	// 모델을 지웁니다. (Destroy)
	void RemoveModel();
	
	// 모델 파일의 Transform을 변경합니다.
	void SetModelTransform(const FTransform& InWorldTransform);

private:
	// 모델을 그린다. (glTFRumtimeAsset을 기반으로 actor을 하나 생성하고, 그 포인터를 들고 있음)
	UFUNCTION()
	void _DrawModel(UglTFRuntimeAsset* InAsset);
	
	// 모델 요청이 완료되면 execute될 delegate
	FglTFRuntimeHttpResponse OnRequestModelCompleted;
	
	// 모델링된 Actor
	UPROPERTY( Transient, VisibleInstanceOnly )
	TWeakObjectPtr<AglTFRuntimeAssetActor> ModelPtr;

	// 모델링 파일의 Transform
	UPROPERTY( Transient )
	FTransform ModelWorldTransform;
};