// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "SpectrEvaluator.h"
#include "SpectrGoal.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class SPECTRAI_API USpectrGoal : public UObject
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, bool> Goal;
		
	//What to change in state if goal has been achieved ?
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, bool> FinishedState;
	
	UPROPERTY(EditAnywhere, Instanced)
		TArray<USpectrConsideration*> Considerations;

	UPROPERTY(EditAnywhere)
		FSpectrEvaluator ScoreEvaluator;

	virtual float QualifyGoal(class USpectrContext* InContext)
	{
		return -1.0f;
	}

	//What Should we do if this goal is selected ?
	virtual void GoalSelected()
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
