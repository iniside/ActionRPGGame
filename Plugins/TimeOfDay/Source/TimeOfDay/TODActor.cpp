// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDay.h"
#include "Net/UnrealNetwork.h"
//#include "DirectionalLightComponent.h"

#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "TODAsset.h"
#include "TODActor.h"

ATODActor::ATODActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TODRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	bReplicates = true;
	SetReplicates(true);
	
	RootComponent = TODRoot;

	AtmosphericFog = ObjectInitializer.CreateDefaultSubobject<UAtmosphericFogComponent>(this, TEXT("Atmospheric Fog"));
	AtmosphericFog->SetMobility(EComponentMobility::Movable);
	AtmosphericFog->AttachTo(RootComponent);

	Sun = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, TEXT("Sun"));
	Sun->SetMobility(EComponentMobility::Movable);
	Sun->Intensity = 5;
	Sun->AttachTo(AtmosphericFog);
	Sun->bUsedAsAtmosphereSunLight = true;
	Sun->bUseRayTracedDistanceFieldShadows = true;
	Sun->bEnableLightShaftOcclusion = true;
	//Sun->SetNetAddressable();
	//Sun->SetIsReplicated(true);

	SunMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Sun Mesh"));
	SunMesh->SetMobility(EComponentMobility::Movable);
	SunMesh->AttachTo(RootComponent);
	SunMesh->SetCastShadow(false);
	SunMesh->bAbsoluteRotation = false;
	SunMesh->bAbsoluteLocation = false;

	SkyLight = ObjectInitializer.CreateDefaultSubobject<USkyLightComponent>(this, TEXT("Sky Light"));
	SkyLight->SetMobility(EComponentMobility::Movable);
	SkyLight->AttachTo(RootComponent);

	PostProcess = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("Post Process"));
	PostProcess->SetMobility(EComponentMobility::Movable);
	PostProcess->bUnbound = true;
	PostProcess->AttachTo(RootComponent);

	HeightFog = ObjectInitializer.CreateDefaultSubobject<UExponentialHeightFogComponent>(this, TEXT("Height Fog"));
	HeightFog->AttachTo(RootComponent);

	NightSky = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Night Sky"));
	NightSky->SetMobility(EComponentMobility::Movable);
	NightSky->SetCastShadow(false);
	NightSky->bAbsoluteRotation = true;

	Moon = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, TEXT("Moon"));
	Moon->SetMobility(EComponentMobility::Movable);
	Moon->Intensity = 1;
	Moon->AttachTo(RootComponent);
	Moon->bUsedAsAtmosphereSunLight = false;
	Moon->bAffectsWorld = false;
	Moon->bUseRayTracedDistanceFieldShadows = true;
	Moon->bEnableLightShaftBloom = true;
	Moon->bEnableLightShaftOcclusion = true;


	MoonMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Moon Mesh"));
	MoonMesh->SetMobility(EComponentMobility::Movable);
	MoonMesh->SetCastShadow(false);
	MoonMesh->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	//if (TODAsset)
	//	SunMeshMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, TODAsset->SunMeshMaterial);

	//range of movement of sun 0 degrees is midday.
	// -90, 90 are sunset, sunrise - depending on sun angle
	//I have added 3 degrees to allow more smooth transition, and margin of error.
	//but technically it should work with -90,90 degrees.
	OutRange = FVector2D(-98, 98);
	MoonOutRnage = FVector2D(-98, 98);
}
void ATODActor::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATODActor, SunPosition);
	DOREPLIFETIME(ATODActor, TODAsset);
}

void ATODActor::OnRep_SunPosition()
{
	ChangeTODSettings();
}

void ATODActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//SetActorRotation(FRotator(SunAngle, 0, SunRotation));
	//Sun->SetRelativeRotation(FRotator(0, SunPosition, 0));
}

void ATODActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//tick only on server.
	//we don't want clients to messup our time of day at any point.
	//if (Role < ROLE_Authority)
	//{

	//}
	//else
	//{
		if (TODAsset)
		{
			if (TODAsset->IsDynamicTODEnabled)
			{
				CurrentTime = (TODAsset->TimeOfDaySpeed*DeltaSeconds) + CurrentTime;
				if (CurrentTime >= 24)
				{
					CurrentTime = 0;
				}
				InRange = FVector2D(TODAsset->SunsetTime, TODAsset->SunriseTime);
				float MapedHour = FMath::GetMappedRangeValue(InRange, OutRange, CurrentTime);
				GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::FormatAsNumber(MapedHour));
				SunPosition = MapedHour;
				ChangeTODSettings();
			}
		}
//	}
}

void ATODActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!TODAsset)
		return;
	if (!MoonMesh|| !SunMesh)
		return;

	InRange = FVector2D(TODAsset->SunsetTime, TODAsset->SunriseTime);
	MoonInRange = FVector2D(TODAsset->MoonsetTime, TODAsset->MoonriseTime);
	CurrentTime = TODAsset->Hour;

	if (CurrentTime == 24)
		CurrentTime = 0;

	//SunRise - 9
	//sun hours = 9,10,11,12,13,14,15,16,17,18
	//SunSet - 18
	//MoonRise - 18
	//Moon Hours = 18,19,20,21,22,24,0,1,2,3,4,5,6,7,8
	//MoonSet - 9

	FVector2D HourRemapIn = FVector2D(0, 24);
	FVector2D HourRemapOutRange = FVector2D(-12, 12);
	float RemapedHours = FMath::GetMappedRangeValue(HourRemapOutRange, HourRemapIn, CurrentTime);;

	float MapedSunPosition = FMath::GetMappedRangeValue(InRange, OutRange, CurrentTime);
	SunPosition = MapedSunPosition;


	MoonMesh->SetStaticMesh(TODAsset->MoonMesh);
	SunMesh->SetStaticMesh(TODAsset->SunMesh);

	ChangeTODSettings();
}

void ATODActor::ChangeTODSettings()
{

	//that's prolly ugly, but I don't have better idea to handle it.
	if (CurrentTime > TODAsset->MoonriseTime)
	{
		MoonPosition = FMath::GetMappedRangeValue(FVector2D(TODAsset->MoonriseTime, 24), FVector2D(-90, 0), CurrentTime);
		NightSky->SetVisibility(true);
	}
	else
	{
		NightSky->SetVisibility(false);
	}
	if (CurrentTime < TODAsset->MoonsetTime)
	{
		MoonPosition = FMath::GetMappedRangeValue(FVector2D(0, TODAsset->MoonsetTime), FVector2D(0, 90), CurrentTime);
		NightSky->SetVisibility(true);
	}
	else
	{
		NightSky->SetVisibility(false);
	}

	FRotator ActorRotation = FRotator::ZeroRotator;
	ActorRotation.Pitch = TODAsset->SunRotation;
	ActorRotation.Yaw = 0;
	ActorRotation.Roll = TODAsset->SunAngle;

	SetActorRotation(ActorRotation);
	//Sun->UnregisterComponent();
	
	//Sun
	FRotator LocalSunRotation;
	LocalSunRotation.Pitch = 0;
	LocalSunRotation.Yaw = SunPosition;
	LocalSunRotation.Roll = 0;

	Sun->SetRelativeRotation(LocalSunRotation);
	Sun->SetIntensity(TODAsset->SunIntensityCurve.EditorCurveData.Eval(CurrentTime));
	Sun->SetLightColor(TODAsset->SunColorCurve.GetLinearColorValue(CurrentTime));

	Sun->UpdateColorAndBrightness();
	SunMesh->SetWorldScale3D(FVector(TODAsset->SunMeshScale));

	//for some reason mesh Pitch and Roll should be in opposite direction 0o.
	
	FRotator SunRotation = FRotator::ZeroRotator;
	SunRotation.Pitch = TODAsset->SunAngle;
	SunRotation.Yaw = SunPosition;
	SunRotation.Roll = TODAsset->SunRotation;
	FVector Location = SunRotation.Vector(); //UKismetMathLibrary::Conv_RotatorToVector(SunRotation);
	Location = (TODAsset->SunMeshDistance * (-1)) * Location;
	//SunMesh->RelativeLocation = Location;
	SunMesh->SetRelativeLocation(Location);
	SunMesh->UpdateComponentToWorld();
	
	//Moon
	Moon->SetRelativeRotation(FRotator(0, MoonPosition, 0));
	Moon->SetIntensity(TODAsset->MoonIntensityCurve.EditorCurveData.Eval(CurrentTime));
	Moon->SetLightColor(TODAsset->MoonColorCurve.GetLinearColorValue(CurrentTime));

	MoonMesh->SetWorldScale3D(FVector(TODAsset->MoonMeshScale));
	FRotator MoonRotation = FRotator::ZeroRotator;
	MoonRotation.Pitch = TODAsset->SunAngle;
	MoonRotation.Yaw = MoonPosition;
	MoonRotation.Roll = TODAsset->SunRotation;

	FVector MoonLocation = MoonRotation.Vector(); //UKismetMathLibrary::Conv_RotatorToVector(SunRotation);
	MoonLocation = (TODAsset->MoonMeshDistance * (-1)) * MoonLocation;
	//SunMesh->RelativeLocation = Location;
	MoonMesh->SetRelativeLocation(MoonLocation);
	MoonMesh->UpdateComponentToWorld();

	Moon->UpdateColorAndBrightness();

	NightSky->SetWorldScale3D(FVector(TODAsset->NightSkyScale));

	SkyLight->SetIntensity(TODAsset->SkyLightIntensity.EditorCurveData.Eval(CurrentTime));
	SkyLight->SetLightColor(TODAsset->SkyLightColor.GetLinearColorValue(CurrentTime));

	SkyLight->UpdateSkyCaptureContents(GetWorld());
	//SkyLight->DoDeferredRenderUpdates_Concurrent();

	//Post Process:
	PostProcess->Settings.bOverride_AutoExposureMinBrightness = true;
	PostProcess->Settings.AutoExposureMinBrightness = TODAsset->AutoExposureMinBrigthness.EditorCurveData.Eval(CurrentTime);
	PostProcess->Settings.bOverride_AutoExposureMaxBrightness = true;
	PostProcess->Settings.AutoExposureMaxBrightness = TODAsset->AutoExposureMaxBrigthness.EditorCurveData.Eval(CurrentTime);
	PostProcess->Settings.bOverride_AutoExposureBias = true;
	PostProcess->Settings.AutoExposureBias = TODAsset->AutoExposureBias.EditorCurveData.Eval(CurrentTime);
	
	PostProcess->MarkRenderStateDirty();
	//Height Fog

	HeightFog->MarkRenderStateDirty();
}