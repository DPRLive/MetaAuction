// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MAItemDisplayer.generated.h"

UCLASS()
class METAAUCTION_API AMAItemDisplayer : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMAItemDisplayer();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	void Init(const uint32 InItemID);
	void MoveCamera(const FVector2D& InPos);
	void ZoomInCamera();
	void ZoomOutCamera();

private:

	float GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const;
	FRotator GetDeltaRotation(float DeltaTime) const;
	FVector GetHitZLocation(const FVector& InStart, const int32 InTrace, const TArray<TWeakObjectPtr<UPrimitiveComponent>>& Ignores) const;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> RotateSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> BackgroundMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> DisplayMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UModelDrawComponent> ModelDrawComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneCaptureComponent2D> DisplayCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	int32 MoveRange;

	UPROPERTY(Transient)
	FVector DefaultCameraLocation;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FVector2D MoveDestination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float MoveInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FRotator RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	uint8 bIsRotate : 1;

	UPROPERTY(Transient)
	float DefaultZoom;

	UPROPERTY(Transient)
	float CurrentZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float ZoomRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float ZoomValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	int32 ZoomInterpSpeed;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> DrawedModel;
};
