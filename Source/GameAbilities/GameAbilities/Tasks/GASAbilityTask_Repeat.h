// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_Repeat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnTaskRepeated);

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_Repeat : public UGASAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
		FGASOnTaskRepeated OnTaskRepeated;
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_Repeat* CreateRepeatTask(UObject* WorldContextObject);
};
