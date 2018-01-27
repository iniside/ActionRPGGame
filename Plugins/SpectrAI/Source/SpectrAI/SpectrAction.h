// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "SpectrAction.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SPECTRAI_API USpectrAction : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		int32 Cost;
	/*
		Tag Name of requirent condition in agent state, Desiared State Value
	*/
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		TMap<FGameplayTag, bool> PreConditions;

	/*
		Tag Name of change applied to agent state is effect is executed, Desiared State Value
	*/
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		TMap<FGameplayTag, bool> Effects;

	UPROPERTY()
		USpectrBrainComponent* OwningBrain;

	/* Override to check if action in rnage of Target/Location to execute */
	virtual bool NativeIsInRange()
	{
		return true;
	}

	virtual void NativeMoveTo(class USpectrBrainComponent* Brain);

	virtual void NativeOnMoveFinished(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain) {}

	virtual void NativeExecute() {};

	virtual float NativeSscore(class USpectrContext* InContext, class AAIController* AIController)
	{
		return Cost;
	}

	virtual float NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController)
	{
		return true;
	}

	void NativeFinished()
	{

	}

	UFUNCTION(BlueprintNativeEvent)
		void Execute(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);
	virtual void Execute_Implementation(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	UFUNCTION(BlueprintNativeEvent)
		float Score(class USpectrContext* InContext, class AAIController* AIController);
	virtual float Score_Implementation(class USpectrContext* InContext, class AAIController* AIController);

	UFUNCTION(BlueprintNativeEvent)
		bool EvaluateCondition(class USpectrContext* InContext, class AAIController* AIController);
	virtual bool EvaluateCondition_Implementation(class USpectrContext* InContext, class AAIController* AIController);
};
