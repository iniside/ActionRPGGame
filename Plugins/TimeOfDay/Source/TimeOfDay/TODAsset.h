// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CurveColor.h"
#include "TODAsset.generated.h"

/*
General idea about cloud distribution:
1. Split sky into grid.
2. Setup cell size (cells have uniform size for example 100x100m, never 100x50).
3. Each cell can contain list of clouds.
4. If cell doesn't have any clouds DoesHaveClouds = false.
5. There is not limit how much clouds can be in single cell. Though I suspect no more than 3.
6. There is no upper limit to grid size.

How does cloud distribution look ?
1. User create list of available clouds.
2. Setup grid size (sizeOfCell*NumberOfCells).
3. Set how much clouds can be in each cell.
4. Set how much clouds should be spawned.

Now when asset is placed on level, particle systems representing clouds will be randomly distributed over
grid. There is no particular algorithm i think of that would make them look like something, so 
distribution should be pure random.
If the the cell reached maximum amount of clouds, algorhitm will try to find another one.
If there is for some reason more clouds than cells, and alll cells are full, new clouds won't be spawned.

You can regenerate clouds in TOD Editor. Cloud position will be serialized and saved along with asset. I think ;)
Might not work.

This means, if you want to get nice sky, you need nice clouds, which means you need good particle system.

Clouds are particle systems. They are NOT volumetric clouds. Although nothing is stopping user
from creating ones ;). The once provided by default are based on clouds from ShooterGame sample.
I personally find them pleasing enough at big distances.

*/

/*
Possible features:
1. Planetary system. Well not exactly space game planetary system ;), but ability to add X number
of planets, and then make them rotate over different orbits with different speeds, and being of
different sizes.
*/
USTRUCT()
struct FCloudCell
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Maximum amount of cloud particle systems in cell.
	*/
	UPROPERTY(EditAnywhere, Category = "Clouds")
		int32 MaxCloudCount;
	/*
		Size of Grid cell in meters.
	*/
	UPROPERTY(EditAnywhere, Category = "Clouds")
		int32 CellSize;

	int32 CurrentCloudCount;

};
/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules 
 * within this plugin.
 */
UCLASS(meta = (DisplayName = "Time Of Day"), BlueprintType, Blueprintable)
class TIMEOFDAY_API UTODAsset : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Of Day")
		bool IsDynamicTODEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, UIMax = 24, ClampMin = 0, ClampMax = 24), Category = "Time Of Day")
		float Hour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Of Day")
		float TimeOfDaySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
		float SunAngle;
	UPROPERTY()
		float SunPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
		float SunRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, UIMax = 24, ClampMin = 0, ClampMax = 24), Category = "Sun")
		float SunsetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, UIMax = 24, ClampMin = 0, ClampMax = 24), Category = "Sun")
		float SunriseTime;

	UPROPERTY(EditAnywhere, Category = "Sun")
		FRuntimeFloatCurve SunIntensityCurve;

	UPROPERTY(EditAnywhere, Category = "Sun")
		FRuntimeCurveLinearColor SunColorCurve;

	UPROPERTY()
		float SunTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
		float SunMeshScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
		float SunMeshDistance;

	UPROPERTY(EditAnywhere, Category = "Sun")
		UStaticMesh* SunMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
		UMaterialInterface* SunMeshMaterial;


	//SkyLight
	UPROPERTY(EditAnywhere, Category = "SkyLight")
		FRuntimeFloatCurve SkyLightIntensity;

	UPROPERTY(EditAnywhere, Category = "SkyLight")
		FRuntimeCurveLinearColor SkyLightColor;

	/* Moon */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, UIMax = 24, ClampMin = 0, ClampMax = 24), Category = "Moon")
		float MoonsetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, UIMax = 24, ClampMin = 0, ClampMax = 24), Category = "Moon")
		float MoonriseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon")
		float MoonAngle;
	UPROPERTY()
		float MoonPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon")
		float MoonRotation;

	UPROPERTY(EditAnywhere, Category = "Moon")
		FRuntimeFloatCurve MoonIntensityCurve;
	UPROPERTY(EditAnywhere, Category = "Moon")
		FRuntimeCurveLinearColor MoonColorCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon")
		float MoonMeshScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon")
		float MoonMeshDistance;

	UPROPERTY(EditAnywhere, Category = "Moon")
		UStaticMesh* MoonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon")
		UMaterialInterface* MoonMeshMaterial;

	UPROPERTY(EditAnywhere, Category = "Night Sky")
		float NightSkyScale;
	UPROPERTY(EditAnywhere, Category = "Night Sky")
		UStaticMesh* NightSkyMesh;


	//PostProcess

	UPROPERTY(EditAnywhere, meta=(DisplayName="Min Brigthness"), Category = "Post Process")
		FRuntimeFloatCurve AutoExposureMinBrigthness;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Max Brigthness"), Category = "Post Process")
		FRuntimeFloatCurve AutoExposureMaxBrigthness;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Exposure Bias"), Category = "Post Process")
		FRuntimeFloatCurve AutoExposureBias;

	//Exponential Height Fog.
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve FogDensity;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeCurveLinearColor FogInscatteringColor;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve FogHeightFalloff;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve FogMaxOpacity;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve FogStartDistance;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve DirectionalInnscatteringExponenet;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeFloatCurve DirectionalInnscatteringStartDistance;
	UPROPERTY(EditAnywhere, Category = "Height Fog")
		FRuntimeCurveLinearColor DirectionalInnscatteringColor;

	//Atmospheric Fog
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve SunMultiplier;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve FogMultiplier;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve DensityMultiplier;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve DensityOffset;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve DistanceScale;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve AltitudeScale;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve DistanceOffset;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve GroundOffset;
	UPROPERTY(EditAnywhere, Category = "Atmospheric Fog")
		FRuntimeFloatCurve StartDistance;
	/* Clouds */
	//TArray<
};

