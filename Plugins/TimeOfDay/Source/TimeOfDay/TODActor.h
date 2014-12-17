// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "TODActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDMDOnSunset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDMDOnSunrise);

//Delegates for rain
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTODOnRainStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTODOnRainEnd);

//Delegates for snow
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTODOnSnowStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTODOnSnowEnd);

//General delegate indicating if weather changed, but do not indicate how.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTODOnOnWeatherChanged);
/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules 
 * within this plugin.
 */


UCLASS(meta = (DisplayName = "Time Of Day"), BlueprintType, Blueprintable)
class TIMEOFDAY_API ATODActor : public AActor
{
	GENERATED_UCLASS_BODY()
public:

	virtual void PostInitializeComponents() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(ReplicatedUsing=OnRep_SunPosition)
		float SunPosition;

	UPROPERTY()
		float MoonPosition;

	UFUNCTION()
		void OnRep_SunPosition();

	UPROPERTY(Replicated)
		class UTODAsset* TODAsset;

	UPROPERTY(VisibleAnywhere, Category = "Root")
		USceneComponent* TODRoot;
	UPROPERTY(VisibleAnywhere, Category="Sun")
		UDirectionalLightComponent* Sun;

	UPROPERTY(VisibleAnywhere, Category = "Sun")
		UStaticMeshComponent* SunMesh;

	UPROPERTY(VisibleAnywhere, Category = "Sky Light")
		USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, Category = "Post Process")
		UPostProcessComponent* PostProcess;

	UPROPERTY(VisibleAnywhere, Category = "Atmospheric Fog")
		UAtmosphericFogComponent* AtmosphericFog;
	UPROPERTY(VisibleAnywhere, Category = "Height Fog")
		UExponentialHeightFogComponent* HeightFog;

	UPROPERTY(VisibleAnywhere, Category = "Moon")
		UDirectionalLightComponent* Moon;
	UPROPERTY(VisibleAnywhere, Category = "Moon")
		UStaticMeshComponent* MoonMesh;

	UPROPERTY(VisibleAnywhere, Category = "Night Sky")
		UStaticMeshComponent* NightSky;

	UPROPERTY()
		float CurrentTime;
private:
	float SunAngleMultiplier;
	FVector2D InRange;
	FVector2D OutRange;
	FVector2D MoonInRange;
	FVector2D MoonOutRnage;

	UPROPERTY()
		UMaterialInstanceDynamic* SunMeshMaterial;

private:
	void ChangeTODSettings();

};

