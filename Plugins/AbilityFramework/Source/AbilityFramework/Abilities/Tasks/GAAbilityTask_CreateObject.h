// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_CreateObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASSpawnObjectDelegate, class UObject*, SpawnedObject);

/**
 *
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_CreateObject : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASSpawnObjectDelegate Success;
	UPROPERTY(BlueprintAssignable)
		FGASSpawnObjectDelegate Failure;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_CreateObject* CreateObject(UGAAbilityBase* WorldContextObject,
			FName InTaskName, TSubclassOf<class UObject> Class, UObject* Outer);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		bool BeginSpawningActor(UGAAbilityBase* WorldContextObject, TSubclassOf<class UObject> Class, class UObject*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		void FinishSpawningActor(UGAAbilityBase* WorldContextObject, class UObject* SpawnedActor);
};
