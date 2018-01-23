// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "SpectrGoal.generated.h"

/**
 * 
 */
UCLASS()
class SPECTRAI_API USpectrGoal : public UObject
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, bool> Goal;
		
	//What to change in state if goal has been achieved ?
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, bool> FinishedState;
		
	//What Should we do if this goal is selected ?
	virtual void SetupState()
	{
		
	}
		
	//Called when goal has been selected, just before plan is executed.
	virtual void GoalStarted()
	{
		
	}
		
	//called just after plan finished execution
	virtual void GoalFinished()
	{
		
	}
		
	//called if plan for some reason cannot be finished and is aborted.
	virtual void GoalAborted()
	{
		
	}
	
	
};
