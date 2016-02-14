// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASCueActor.h"


// Sets default values
AGASCueActor::AGASCueActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;
}

// Called when the game starts or when spawned
void AGASCueActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGASCueActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

