// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemActorBase.h"
#include "ARItemPickupBase.generated.h"

UCLASS()
class ACTIONRPGGAME_API AARItemPickupBase : public AIFItemActorBase
{
	GENERATED_BODY()

	UPROPERTY()
		class AARItemSpawnerBase* SpawnedBy;
public:	
	// Sets default values for this actor's properties
	AARItemPickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	inline void SetSpawnedBy(class AARItemSpawnerBase* InSpawnedBy) { SpawnedBy = InSpawnedBy; }

	
	virtual void OnItemPicked() override;
};
