// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASEffectActor.h"


// Sets default values
AGASEffectActor::AGASEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGASEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGASEffectActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

