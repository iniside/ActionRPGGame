// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARItemSpawnerBase.generated.h"


/*
	1. Should only store templates.
	2. Should ask databse about item to spawn.
	3. Should not be placed manually.
*/
UCLASS()
class ACTIONRPGGAME_API AARItemSpawnerBase : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Items")
		TSubclassOf<class AARItemPickupBase> ItemsToSpawn;

	/*
		If <= 0, no respawn. Otherwise time after item will be respawned.
	*/
	UPROPERTY(EditAnywhere, Category = "Items")
		float RespawnTime;

	FTimerHandle RespawnHandle;
public:	
	// Sets default values for this actor's properties
	AARItemSpawnerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnItemPicked();
	UFUNCTION()
		void HandleRespawn();
	
};
