// Fill out your copyright notice in the Description page of Project Settings.

#include "AREnemySpawner.h"
#include "Engine/World.h"
#include "AI/ARAICharacter.h"


// Sets default values
AAREnemySpawner::AAREnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxSpawned = 7;
	BatchSpawn = 1;
	TimeBetweenSpawns = 0.3;
	MinRespawn = 4;
}

// Called when the game starts or when spawned
void AAREnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	SetupSpawner();
}

void AAREnemySpawner::SetupSpawner()
{
	if (Role == ROLE_Authority)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AAREnemySpawner::HandleSpawn);

		FTimerManager& Timer = GetWorld()->GetTimerManager();
		Timer.SetTimer(SpawnerHandle, Delegate, TimeBetweenSpawns, true);
	}
}

// Called every frame
void AAREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAREnemySpawner::OnEnemyKilled(AARAICharacter* InEnemy)
{
	if (SpawnedEnemies.Num() > 0)
	{
		SpawnedEnemies.Remove(InEnemy);
	}
	if (SpawnedEnemies.Num() < MinRespawn)
	{
		SetupSpawner();
	}
}

void AAREnemySpawner::HandleSpawn()
{
	if (SpawnPositions.Num() < MaxSpawned)
	{
		return;
	}
	if (SpawnedEnemies.Num() >= MaxSpawned)
	{
		FTimerManager& Timer = GetWorld()->GetTimerManager();
		Timer.ClearTimer(SpawnerHandle);
		return;
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for (int32 Idx = 0; Idx < BatchSpawn; Idx++)
	{
		FTransform Transform;
		const FVector SpawnPos = SpawnPositions[SpawnedEnemies.Num()] + GetActorLocation();
		Transform.SetLocation(SpawnPos);
		Transform.SetScale3D(FVector(1));

		AARAICharacter* Enemy = GetWorld()->SpawnActor<AARAICharacter>(EnemyClass, Transform, SpawnParams);
		Enemy->OnSpawned(this);
		SpawnedEnemies.Add(Enemy);
	}
}