// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/MAItemDisplayer.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SceneCaptureComponent2D.h>
#include <Engine/CanvasRenderTarget2D.h>

AMAItemDisplayer::AMAItemDisplayer()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	RotateSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RotateScene"));
	RotateSceneComponent->SetupAttachment(RootComponent);

	BackgroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackgroundMesh"));
	BackgroundMesh->SetupAttachment(RootComponent);

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(RotateSceneComponent);

	DisplayCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("DisplayCamera"));
	DisplayCamera->SetupAttachment(RootComponent);
	DisplayCamera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	DisplayCamera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	
	MoveRange = 1000.0f;
	InterpSpeed = 10.0f;
	RotationRate = FRotator(0.0f, 0.0f, 60.0f);
	bIsRotate = true;
}

void AMAItemDisplayer::BeginPlay()
{
	Super::BeginPlay();

	DisplayCamera->ShowOnlyActors.Emplace(this);
	DefaultCameraLocation = DisplayCamera->GetComponentLocation();
}

void AMAItemDisplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate Camera
	if (bIsRotate)
	{
		RotateSceneComponent->SetRelativeRotation(RotateSceneComponent->GetRelativeRotation() + GetDeltaRotation(DeltaTime));
	}

	// Move Camera
	const FVector& CameraLocation = DisplayCamera->GetComponentLocation();
	const FVector& UpVector = DisplayCamera->GetUpVector();
	const FVector& RightVector = DisplayCamera->GetRightVector();
	FVector NewLocation = DefaultCameraLocation;
	NewLocation += UpVector * MoveDestination.Y;
	NewLocation += RightVector * MoveDestination.X;
	DisplayCamera->SetWorldLocation(FMath::VInterpTo(CameraLocation, NewLocation, DeltaTime, InterpSpeed));
}

void AMAItemDisplayer::Init(UStaticMesh* NewMesh)
{
	DisplayMesh->SetStaticMesh(NewMesh);
	RotateSceneComponent->SetRelativeRotation(FRotator::ZeroRotator);

	/*
	TArray<TWeakObjectPtr<UPrimitiveComponent>> Ignores;
	Ignores.Emplace(DisplayMesh);
	const FVector GroundLocation = GetHitZLocation(GetActorLocation(), -2000.0f, Ignores);
	Ignores[0] = BackgroundMesh;
	const FVector FallLocation = GetHitZLocation(GetActorLocation(), 2000.0f, Ignores);
	float Delta = FallLocation.Z - GroundLocation.Z;

	FVector NewDisplayMeshLocation = DisplayMesh->GetRelativeLocation();
	NewDisplayMeshLocation.Z -= Delta;
	DisplayMesh->SetRelativeLocation(NewDisplayMeshLocation);

	FVector NewDisplayCameraLocation = DisplayCamera->GetRelativeLocation();
	NewDisplayCameraLocation.Z -= Delta;
	DisplayCamera->SetRelativeLocation(NewDisplayCameraLocation);
	*/
}

float AMAItemDisplayer::GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const
{
	return (InAxisRotationRate >= 0.f) ? FMath::Min(InAxisRotationRate * DeltaTime, 360.f) : 360.f;
}

FRotator AMAItemDisplayer::GetDeltaRotation(float DeltaTime) const
{
	return FRotator(GetAxisDeltaRotation(RotationRate.Pitch, DeltaTime), 
		GetAxisDeltaRotation(RotationRate.Yaw, DeltaTime), 
		GetAxisDeltaRotation(RotationRate.Roll, DeltaTime));
}

void AMAItemDisplayer::MoveCamera(const FVector2D& InPos)
{
	MoveDestination = InPos * MoveSpeed;
	MoveDestination.X = FMath::Clamp(MoveDestination.X, -MoveRange, MoveRange);
	MoveDestination.Y = FMath::Clamp(MoveDestination.Y, -MoveRange, MoveRange);
}

FVector AMAItemDisplayer::GetHitZLocation(const FVector& InStart, const int32 InTrace, const TArray<TWeakObjectPtr<UPrimitiveComponent>>& Ignores) const
{
	FVector End = InStart;
	End.Z += InTrace;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredComponents(Ignores);
	FCollisionShape Shape;
	Shape.ShapeType = ECollisionShape::Box;
	Shape.MakeBox(FVector(100.0f, 100.0f, 100.0f));
	GetWorld()->SweepSingleByChannel(Hit, InStart, End, FQuat::Identity, ECC_Visibility, Shape, Params);
	return Hit.bBlockingHit ? Hit.ImpactPoint : InStart;
}