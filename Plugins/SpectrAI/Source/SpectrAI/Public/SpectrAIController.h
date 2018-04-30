// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "SpectrAIController.generated.h"

//Consideration (calculates score)
//--normalized 0-1
//--calculated against curve

//Qualifier
//--Contains Considerations
//--defines how score Considerations should be combined
//--return score.

//Decision (what should I do?)
//--contain exactly ONE action to execute if decision wins

//Decision Score Evaluator (Selector)
//--containts multiple considerations
//--all condsiderations are calculated with given context
//--they are summed (or multiplied)
//--contains single decision to execute.



/**
 * 
 */
UCLASS()
class SPECTRAI_API ASpectrAIController : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpectrBrainComponent* SpectrBrain;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAIPerceptionComponent* AIPerception;


	ASpectrAIController(const FObjectInitializer& ObjectInitializer);

	
};
