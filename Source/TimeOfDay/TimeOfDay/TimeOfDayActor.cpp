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
	bReplicates = true;
	TODRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FTODObjectNames::TODRootName);
	RootComponent = TODRoot;
	Sun = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, FTODObjectNames::SunName);
	Sky = ObjectInitializer.CreateDefaultSubobject<USkyLightComponent>(this, FTODObjectNames::SkyName);
	PostProcess = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, FTODObjectNames::PostProcessName);
	HeightFog = ObjectInitializer.CreateDefaultSubobject<UExponentialHeightFogComponent>(this, FTODObjectNames::HeightFogName);
	AtmosphericFog = ObjectInitializer.CreateDefaultSubobject<UAtmosphericFogComponent>(this, FTODObjectNames::AtmosphericFogName);

	SunMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FTODObjectNames::SunMeshName);
	MoonMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FTODObjectNames::MoonMeshName);
	
	AtmosphericFog->AttachTo(RootComponent);
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
	
}

// Called every frame
void ATimeOfDayActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

