// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TimeOfDayActor.generated.h"

struct FSphercialCoords
{
public:
	//because it's easier to read from wikipedia..
	float Theta; //latitude //azimuth
	float Phi; //longitiude //polar angle
	float Rho; //r //distance

	FSphercialCoords()
		: Theta(0),
		Phi(0),
		Rho(0)
	{};
	FSphercialCoords(float ThetaIn, float PhiIn, float RhoIn)
		: Theta(ThetaIn),
		Phi(PhiIn),
		Rho(RhoIn)
	{};

	FSphercialCoords(const FVector& VectorIn)
	{
		Rho = VectorIn.Size();
		Theta = FMath::Acos(VectorIn.Z / Rho);
		Phi = FMath::Atan(VectorIn.Y / VectorIn.X);
	};

	void SetCoords(float ThetaIn, float PhiIn, float RhoIn)
	{
		Theta = ThetaIn;
		Phi = PhiIn;
		Rho = RhoIn;
	}
	FVector GetVector()
	{
		FVector VectorOut = FVector::ZeroVector;
		VectorOut.X = Rho * FMath::Sin(Theta) * FMath::Cos(Phi);
		VectorOut.Y = Rho * FMath::Sin(Theta) * FMath::Sin(Phi);
		VectorOut.Z = Rho * FMath::Cos(Theta);
		return VectorOut;
	};
};

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
	UPROPERTY(EditAnywhere, Category = "Config")
		FRuntimeFloatCurve Declination;
	UPROPERTY(EditAnywhere, Category = "Config")
		bool Runtime;
	/* Lenght of full day. */
	UPROPERTY(EditAnywhere, Category = "Day Config")
		float FullCycleDuration;
	/* Days in Year. */
	UPROPERTY(EditAnywhere, Category = "Day Config")
		int32 YearCycleDuration;
	UPROPERTY(EditAnywhere, Category = "Day Config")
		float DayDuration;
	UPROPERTY(EditAnywhere, Category = "Day Config")
		float Latitude;
	UPROPERTY(EditAnywhere, Category = "Day Config")
		float SunDeclination;
	UPROPERTY(EditAnywhere, Category = "Day Config")
		float Distance;
	UPROPERTY(EditAnywhere, Category = "Time")
		int32 CurrentDay;
	UPROPERTY(EditAnywhere, Category = "Time")
		int32 Hour;
	UPROPERTY(EditAnywhere, Category = "Time")
		int32 Minutes;
	UPROPERTY(EditAnywhere, Category = "Time")
		int32 Seconds;
	UPROPERTY(EditAnywhere, Category = "Time")
		float TimeDivider;

	float SunDrive;
	float DayNightFlip;
	float Shift;
	float Lenght;
	float EquatorFlip;

	float SunPitch;
	float SunYaw;

	UPROPERTY(EditAnywhere, Category = "Debug")
		float Rotation; //longitude
	UPROPERTY(EditAnywhere, Category = "Debug")
		float Angle; //latitude

	UPROPERTY(EditAnywhere, Category = "Debug")
		float Scale;
	UPROPERTY(EditAnywhere, Category = "Debug")
		float SunRise;
	UPROPERTY(EditAnywhere, Category = "Debug")
		float SunSet;



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

	float CalculateTime();

	void CalculateSunTrajectory();

	void UpdateTimeOfDay();
	virtual void OnConstruction(const FTransform& Transform) override;
};
