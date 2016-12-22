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
	const static FName SkyLightName;
	const static FName PostProcessName;
	const static FName HeightFogName;
	const static FName AtmosphericFogName;
	const static FName SkySphereName;
	const static FName MoonMeshName;
};

UCLASS()
class TIMEOFDAY_API ATimeOfDayActor : public AActor
{
	GENERATED_BODY()
public:
	/* For Earth it's 24h */
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 DayLenght;
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 YearDuration;

	/* How Fast should time of day progress in seconds */
	UPROPERTY(EditAnywhere, Category = "Config")
		float TimeSpeed;

	UPROPERTY(EditAnywhere, Category = "Config")
		int32 Hour;
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 Minutes;
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 Seconds;

	UPROPERTY(EditAnywhere, Category = "Config")
		float Latitude;
	
	UPROPERTY(EditAnywhere, Category = "Config")
		FRuntimeFloatCurve Declination;
	
	UPROPERTY(EditAnywhere, Category = "Config")
		float Time;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		USceneComponent* TODRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		UDirectionalLightComponent* Sun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		UStaticMeshComponent* SkySphere;

protected:
	UPROPERTY()
		UMaterialInstanceDynamic* SkyMID;

	float CurrentDynamicTime;
public:	
	// Sets default values for this actor's properties
	ATimeOfDayActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void CalculateSunPosition(float CurrentTimeIn);

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform& Transform) override;
};
