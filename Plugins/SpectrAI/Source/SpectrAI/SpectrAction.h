// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "SpectrAction.generated.h"

/**
 *	Default implmenetations of Native* functions call to either blueprint events
 *	or blueprint Native functions which can be overriden from blueprint. 
 *	Though they do provide default implementation (where possible).
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

	UPROPERTY(BlueprintReadOnly, Category = "Spectr AI")
		USpectrBrainComponent* OwningBrain;

	/* Override to check if action in rnage of Target/Location to execute */
	virtual bool NativeIsInRange(class AAIController* AIController)
	{
		return true;
	}

	virtual void NativeMoveTo(class USpectrBrainComponent* Brain);

	virtual void NativeOnMoveFinished(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	UFUNCTION(BlueprintNativeEvent)
		void  MoveTo(class USpectrBrainComponent* Brain);
	virtual void MoveTo_Implementation(class USpectrBrainComponent* Brain);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spectr AI")
		void OnMoveFinished(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	UFUNCTION(BlueprintCallable, Category = "Spectr AI")
		void FinishMove(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	/* 
		Call Super::NativeExecute if you want to execute OnExecuted BP event, or call it manually.

		Remember to call NativeFinished() somewhere, to indicate that Action finished execution.
	*/
	virtual void NativeExecute(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	virtual float NativeScore(class USpectrContext* InContext, class AAIController* AIController);

	virtual bool NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController);
	
	/*
		Remember to call it always after Action has finished execution. Otherwise Action Queue will be stuck.
	*/
	void NativeFinished();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectr AI")
		void OnExecute(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain);

	UFUNCTION(BlueprintCallable, Category = "Spectr AI")
		void ActionFinished();

	UFUNCTION(BlueprintNativeEvent)
		float Score(class USpectrContext* InContext, class AAIController* AIController);
	virtual float Score_Implementation(class USpectrContext* InContext, class AAIController* AIController);

	UFUNCTION(BlueprintNativeEvent)
		bool EvaluateCondition(class USpectrContext* InContext, class AAIController* AIController);
	virtual bool EvaluateCondition_Implementation(class USpectrContext* InContext, class AAIController* AIController);




	UFUNCTION(BlueprintCallable)
		void MoveToTarget(AActor* Target, float MinimumDistance);
};
