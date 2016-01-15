// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TimeOfDayActor.generated.h"

struct FTODObjectNames
{
	const static FName TODRootName;
	const static FName SunName;
	const static FName SkyName;
	const static FName PostProcessName;
	const static FName HeightFogName;
	const static FName AtmosphericFogName;
	const static FName SunMeshName;
	const static FName MoonMeshName;
};

UCLASS()
class TIMEOFDAY_API ATimeOfDayActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		USceneComponent* TODRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		UDirectionalLightComponent* Sun;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		USkyLightComponent* Sky;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		UPostProcessComponent* PostProcess;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		UExponentialHeightFogComponent* HeightFog;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
		UAtmosphericFogComponent* AtmosphericFog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		UStaticMeshComponent* SunMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		UStaticMeshComponent* MoonMesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sun")
		float SunMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sun")
		FRuntimeFloatCurve SunTemperature;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sun")
		FRuntimeFloatCurve SunIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sun")
		FRuntimeCurveLinearColor SunColor;
public:	
	// Sets default values for this actor's properties
	ATimeOfDayActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
