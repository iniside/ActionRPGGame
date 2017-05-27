// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_SpawnActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASSpawnActorDelegate, AActor*, SpawnedActor);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_SpawnActor : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASSpawnActorDelegate Success;
	UPROPERTY(BlueprintAssignable)
		FGASSpawnActorDelegate Failure;
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGAAbilityTask_SpawnActor* SpawnActor(UObject* WorldContextObject, 
			FName InTaskName, TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<AActor> Class, AActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, AActor* SpawnedActor);

	virtual void Activate() override;
};