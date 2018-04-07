// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_Repeat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnTaskRepeated);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_Repeat : public UGAAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
		FGASOnTaskRepeated OnTaskRepeated;
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_Repeat* CreateRepeatTask(UGAAbilityBase* WorldContextObject,
			FName InTaskName);
};
