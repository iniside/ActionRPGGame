// Fill out your copyright notice in the Description page of Project Settings.

#include "TimeOfDay.h"
#include "TimeOfDayActor.h"

const FName FTODObjectNames::TODRootName = TEXT("TODRoot");
const FName FTODObjectNames::SunName = TEXT("Sun");
const FName FTODObjectNames::SkyName = TEXT("Sky");
const FName FTODObjectNames::PostProcessName = TEXT("PostProcess");
const FName FTODObjectNames::HeightFogName = TEXT("HeightFog");
const FName FTODObjectNames::AtmosphericFogName = TEXT("AtmosphericFog");
const FName FTODObjectNames::SunMeshName = TEXT("SunMesh");
const FName FTODObjectNames::MoonMeshName = TEXT("MoonMesh");
// Sets default values
ATimeOfDayActor::ATimeOfDayActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	TODRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FTODObjectNames::TODRootName);
	RootComponent = TODRoot;
	Sun = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, FTODObjectNames::SunName);
	Sky = ObjectInitializer.CreateDefaultSubobject<USkyLightComponent>(this, FTODObjectNames::SkyName);
	PostProcess = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, FTODObjectNames::PostProcessName);
	HeightFog = ObjectInitializer.CreateDefaultSubobject<UExponentialHeightFogComponent>(this, FTODObjectNames::HeightFogName);
	//AtmosphericFog = ObjectInitializer.CreateDefaultSubobject<UAtmosphericFogComponent>(this, FTODObjectNames::AtmosphericFogName);

	SunMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FTODObjectNames::SunMeshName);
	MoonMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FTODObjectNames::MoonMeshName);
	EquatorFlip = 1;
	DayNightFlip = 1;
	TimeDivider = 1;
	Shift = 0;
	//AtmosphericFog->AttachTo(RootComponent);
	Sun->AttachTo(AtmosphericFog);
	Sky->AttachTo(RootComponent);
	PostProcess->AttachTo(RootComponent);
	HeightFog->AttachTo(RootComponent);
	SunMesh->AttachTo(RootComponent);
	MoonMesh->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void ATimeOfDayActor::BeginPlay()
{
	Super::BeginPlay();
	if (!Runtime)
		return;

}

// Called every frame
void ATimeOfDayActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//SunDrive = (CalculateTime() / 24) * DayDuration;
	SunDrive = SunDrive + (DeltaTime / TimeDivider);
	if (SunDrive >= DayDuration)
	{
		SunDrive = 0;
	}
	CalculateSunTrajectory();
	float EquatorCheck = 90 - Latitude + SunDeclination;
	if (EquatorCheck > 90)
	{
		EquatorFlip = -1;
	}
	else
	{
		EquatorFlip = 1;
	}
	FRotator SunRotation(SunPitch, SunYaw, 0);
	Sun->SetRelativeRotation(SunRotation);

	FVector Forward = SunRotation.Vector();
	FVector SunMeshLocation = Forward * Distance * -1;
	SunMesh->SetRelativeLocation(SunMeshLocation);
	SunMesh->SetWorldScale3D(FVector(Scale));

}
float ATimeOfDayActor::CalculateTime()
{
	float ReturnValue = (float)FMath::Clamp<int32>(Hour, 0, FullCycleDuration)
		+ ((float)FMath::Clamp<int32>(Minutes, 0, 60) / 60)
		+ ((float)FMath::Clamp<int32>(Seconds, 0, 60) / 3600);
	return ReturnValue;
}
void ATimeOfDayActor::CalculateSunTrajectory()
{
	FVector2D YearRangeIn(0, YearCycleDuration);
	FVector2D YearRangeOut(0, 1);
	float YearVal = FMath::GetMappedRangeValueClamped(YearRangeIn, YearRangeOut, CurrentDay);
	float CurrentDeclination = Declination.GetRichCurveConst()->Eval(YearVal);
	
	float SunHalfDay = (FMath::Acos(FMath::Tan(FMath::DegreesToRadians(Latitude)) *
		FMath::Tan(FMath::DegreesToRadians(CurrentDeclination * -1)))) /
		(FMath::DegreesToRadians(360));

	float SunHalfNight = (FMath::Acos(FMath::Tan(FMath::DegreesToRadians(Latitude)) 
		* FMath::Tan(FMath::DegreesToRadians(CurrentDeclination))))
		/ (FMath::DegreesToRadians(360));

	if (SunDrive <= SunHalfNight)
	{
		DayNightFlip = -1;
		Shift = 0;
		Lenght = SunHalfNight;
	}
	else if ((SunDrive > SunHalfNight) && (SunDrive <= (SunHalfDay * 2 + SunHalfNight)))
	{
		DayNightFlip = 1;
		Shift = DayDuration / 2;
		Lenght = SunHalfDay;
	}
	else if (SunDrive > (SunHalfDay * 2 + SunHalfNight))
	{
		DayNightFlip = -1;
		Shift = DayDuration;
		Lenght = SunHalfNight;
	}

	float PitchLatitude = 90 - FMath::Abs(Latitude - (CurrentDeclination * DayNightFlip));
	float PitchLatitudeSqrt = FMath::Sqrt(PitchLatitude);
	float LenghtShift = ((SunDrive - Shift) / Lenght);
	float LenghtShiftLatitiude = FMath::Square(LenghtShift*PitchLatitudeSqrt);
	float LenghtShiftLatitiudeFlip = LenghtShiftLatitiude * DayNightFlip;
	float PitchLatitiudeFlip = PitchLatitude * DayNightFlip;
	SunPitch = LenghtShiftLatitiudeFlip - PitchLatitiudeFlip;

	SunYaw = (((SunDrive * 360) / DayDuration) * EquatorFlip) + ((1 - EquatorFlip) * 90);
}
void ATimeOfDayActor::UpdateTimeOfDay()
{
	if (SunDrive >= DayDuration)
	{
		SunDrive = 0;
	}
	CalculateSunTrajectory();
	float EquatorCheck = 90 - Latitude + SunDeclination;
	if (EquatorCheck > 90)
	{
		EquatorFlip = -1;
	}
	else
	{
		EquatorFlip = 1;
	}
	FRotator SunRotation(SunPitch, SunYaw, 0);
	Sun->SetRelativeRotation(SunRotation);

	FVector Forward = SunRotation.Vector();
	FVector SunMeshLocation = Forward * Distance * -1;
	SunMesh->SetRelativeLocation(SunMeshLocation);
	SunMesh->SetWorldScale3D(FVector(Scale));
}
void ATimeOfDayActor::OnConstruction(const FTransform& Transform)
{
	SunDrive = (CalculateTime() / FullCycleDuration) * DayDuration;

	if (SunDrive >= DayDuration)
	{
		SunDrive = 0;
	}
	CalculateSunTrajectory();
	float EquatorCheck = 90 - Latitude + SunDeclination;
	if (EquatorCheck > 90)
	{
		EquatorFlip = -1;
	}
	else
	{
		EquatorFlip = 1;
	}
	FRotator SunRotation(SunPitch, SunYaw, 0);
	Sun->SetRelativeRotation(SunRotation);

	FVector Forward = SunRotation.Vector();
	FVector SunMeshLocation = Forward * Distance * -1;
	SunMesh->SetRelativeLocation(SunMeshLocation);
	SunMesh->SetWorldScale3D(FVector(Scale));

}