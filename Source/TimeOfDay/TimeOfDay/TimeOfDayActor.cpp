// Fill out your copyright notice in the Description page of Project Settings.

#include "TimeOfDay.h"
#include "TimeOfDayActor.h"

const FName FTODObjectNames::TODRootName = TEXT("TODRoot");
const FName FTODObjectNames::SunName = TEXT("Sun");
const FName FTODObjectNames::SkyLightName = TEXT("Sky");
const FName FTODObjectNames::PostProcessName = TEXT("PostProcess");
const FName FTODObjectNames::HeightFogName = TEXT("HeightFog");
const FName FTODObjectNames::AtmosphericFogName = TEXT("AtmosphericFog");
const FName FTODObjectNames::SkySphereName = TEXT("SkySphere");
const FName FTODObjectNames::MoonMeshName = TEXT("MoonMesh");
// Sets default values
ATimeOfDayActor::ATimeOfDayActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	//PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	bReplicates = true;
	TODRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FTODObjectNames::TODRootName);
	RootComponent = TODRoot;

	Sun = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, FTODObjectNames::SunName);
	Sun->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SkyLight = ObjectInitializer.CreateDefaultSubobject<USkyLightComponent>(this, FTODObjectNames::SkyLightName);
	SkyLight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SkySphere = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FTODObjectNames::SkySphereName);
	SkySphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//	MoonMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TimeSpeed = 10;
}

// Called when the game starts or when spawned
void ATimeOfDayActor::BeginPlay()
{
	Super::BeginPlay();
	if (!SkyMID)
	{
		SkyMID = SkySphere->CreateAndSetMaterialInstanceDynamic(0);
	}
}

void ATimeOfDayActor::CalculateSunPosition(float CurrentTimeIn)
{
	float Latitude2 = FMath::DegreesToRadians(Latitude);
	float Elevation = 0;

	float DegreesPerSecond = 360.0f / (24.0f * 60.0f * 60.0f);
	//int32 CurrentTime = (Hour * 60 * 60) + (Minutes * 60) + Seconds;

	float CurrentElevation = DegreesPerSecond * CurrentTimeIn;
	float Time2 = FMath::DegreesToRadians(CurrentElevation);

	float CurrrentDeclination = Declination.GetRichCurve()->Eval(CurrentTimeIn);
	float Declination2 = FMath::DegreesToRadians(CurrrentDeclination);
	FQuat SunRot = Sun->GetComponentRotation().Quaternion();
	//FQuatRotationMatrix
	/*Elevation = FMath::Asin
	(
		(FMath::Cos(Latitude2)
			* FMath::Cos(Declination2)
			* FMath::Cos(Time2))
			+ (FMath::Sin(Declination2)*FMath::Sin(Latitude2))
	);*/
	Elevation = FMath::Asin
	(
		(FMath::Cos(Latitude2)
			* FMath::Cos(Declination2)
			* FMath::Cos(Time2))
		+ (FMath::Sin(Declination2)*FMath::Sin(Latitude2))
	);
	float Azimuth = FMath::Cos
	(
		(
			(
				(FMath::Sin(Elevation) 
					* FMath::Sin(Latitude2))
					- FMath::Sin(Declination2)
			) 
			/ 
			(
				FMath::Cos(Elevation)
				* FMath::Cos(Latitude2)
			)
		)
	);

	Elevation = FMath::RadiansToDegrees(FMath::Sin(Elevation));
	Azimuth = FMath::RadiansToDegrees(FMath::Cos(Azimuth));
	//pitch -elevation
	//yaw azimuth
	//UE_LOG(TimeOfDayLOG, Log, TEXT("Elevation: %f"), Elevation);
	//UE_LOG(TimeOfDayLOG, Log, TEXT("DegreesPerSecond: %f"), DegreesPerSecond);
	//UE_LOG(TimeOfDayLOG, Log, TEXT("Azimuth: %f"), CurrentElevation);
	UE_LOG(TimeOfDayLOG, Log, TEXT("Quaternion: %s"), *SunRot.ToString());
	//FRotator::
	Sun->SetRelativeRotation(FRotator(Elevation, CurrentElevation, 0));

	FRotator SunRotation = Sun->GetComponentRotation();
	FVector SunPosition = SunRotation.Vector();
	SkyMID->SetVectorParameterValue("Light direction", FLinearColor(SunPosition));
	FColor SunColor = Sun->LightColor;
	SkyMID->SetVectorParameterValue("Sun color", FLinearColor(SunColor));
}

// Called every frame
void ATimeOfDayActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (!SkyMID)
	{
		return;
	}
	float SecondsInDay = 86400;
	CurrentDynamicTime = CurrentDynamicTime + (TimeSpeed * DeltaTime);
	if (CurrentDynamicTime > SecondsInDay)
	{
		//progress to nest day ?
		CurrentDynamicTime = 0;
	}

	CalculateSunPosition(CurrentDynamicTime);
}


void ATimeOfDayActor::OnConstruction(const FTransform& Transform)
{
	if (!SkyMID)
	{
		SkyMID = SkySphere->CreateAndSetMaterialInstanceDynamic(0);
	}
	float CurrentTime = (Hour * 60 * 60) + (Minutes * 60) + Seconds;
	CalculateSunPosition(CurrentTime);
}