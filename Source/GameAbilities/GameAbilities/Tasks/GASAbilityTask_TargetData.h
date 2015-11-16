// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_TargetData.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_TargetData : public UGASAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Config")
		float Range;
public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_TargetData* CreateTargetDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, class UGASAbilityMod*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, class UGASAbilityMod* SpawnedActor);
	
};
