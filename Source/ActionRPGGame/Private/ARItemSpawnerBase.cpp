// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemSpawnerBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "ARItemPickupBase.h"


// Sets default values
AARItemSpawnerBase::AARItemSpawnerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AARItemSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ENetRole::ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		const FVector Location = GetActorLocation();

		AARItemPickupBase* SpawnedItem = GetWorld()->SpawnActor<AARItemPickupBase>(ItemsToSpawn, Location, GetActorRotation(), SpawnParams);
		SpawnedItem->SetSpawnedBy(this);
	}
}

// Called every frame
void AARItemSpawnerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARItemSpawnerBase::OnItemPicked()
{
	if (RespawnTime <= KINDA_SMALL_NUMBER)
		return;

	FTimerManager& Timer = GetWorld()->GetTimerManager();

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AARItemSpawnerBase::HandleRespawn);
	Timer.SetTimer(RespawnHandle, Delegate, RespawnTime, false, RespawnTime);

}
void AARItemSpawnerBase::HandleRespawn()
{
	FActorSpawnParameters SpawnParams;
	const FVector Location = GetActorLocation();

	AARItemPickupBase* SpawnedItem = GetWorld()->SpawnActor<AARItemPickupBase>(ItemsToSpawn, Location, GetActorRotation(), SpawnParams);
	SpawnedItem->SetSpawnedBy(this);
}