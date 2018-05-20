// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemPickupBase.h"
#include "ARItemSpawnerBase.h"

// Sets default values
AARItemPickupBase::AARItemPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void AARItemPickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARItemPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARItemPickupBase::OnItemPicked()
{
	SpawnedBy->OnItemPicked();
}