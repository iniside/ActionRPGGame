// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_CreateObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASSpawnObjectDelegate, class UGASAbilityMod*, SpawnedActor);

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

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		class UGASAbilityMod* SpawnObject(TSubclassOf<class UGASAbilityMod> InClass, UObject* Outer);

	
};
