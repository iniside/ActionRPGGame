// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_CreateObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASSpawnObjectDelegate, class UGASAbilityMod*, SpawnedObject);

/**
 *
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_CreateObject : public UGASAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASSpawnObjectDelegate Success;
	UPROPERTY(BlueprintAssignable)
		FGASSpawnObjectDelegate Failure;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_CreateObject* CreateObject(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer);

	//UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
	//class UGASAbilityMod* SpawnObject(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> InClass, UObject* Outer);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, class UGASAbilityMod*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, class UGASAbilityMod* SpawnedActor);
};
