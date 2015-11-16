// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_SpawnActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASSpawnActorDelegate, AActor*, SpawnedActor);

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_SpawnActor : public UGASAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASSpawnActorDelegate Success;
	UPROPERTY(BlueprintAssignable)
		FGASSpawnActorDelegate Failure;
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_SpawnActor* SpawnActor(UObject* WorldContextObject, TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<AActor> Class, AActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, AActor* SpawnedActor);

	
};