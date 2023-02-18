// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AREnemySpawner.generated.h"

UCLASS()
class ACTIONRPGGAME_API AAREnemySpawner : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Enemy")
		TSubclassOf<class AARAICharacter> EnemyClass;
	UPROPERTY(EditAnywhere, Category = "Enemy")
		int32 MaxSpawned;
	UPROPERTY(EditAnywhere, Category = "Enemy")
		int32 BatchSpawn;
	UPROPERTY(EditAnywhere, Category = "Enemy")
		float TimeBetweenSpawns;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget), Category = "Enemy")
		TArray<FVector> SpawnPositions;

	/*
		If enemy count reaches this number start respawning.
	*/
	UPROPERTY(EditAnywhere, Category = "Enemy")
		int32 MinRespawn;

	TSet<AARAICharacter*> SpawnedEnemies;

	FTimerHandle SpawnerHandle;

public:	
	// Sets default values for this actor's properties
	AAREnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupSpawner();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnEnemyKilled(AARAICharacter* InEnemy);

	UFUNCTION()
		void HandleSpawn();
	
};
