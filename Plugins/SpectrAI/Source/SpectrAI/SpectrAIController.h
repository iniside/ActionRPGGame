// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrConsideration
{
	GENERATED_BODY()
public:
	virtual float Score() const { return 0; }
	virtual ~FSpectrConsideration()
	{}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrQualifier
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FSpectrConsideration> Considerations;
public:
	virtual ~FSpectrQualifier()
	{}
	virtual float Qualify() const
	{ 
		float TotalScore = 0;
		for (const FSpectrConsideration& Consideration : Considerations)
		{
			TotalScore += Consideration.Score();
		}
		return TotalScore;
	}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrDecision
{
	GENERATED_BODY()
public:
	mutable float Score;
	void SetScore(float InScore) const
	{
		Score = InScore;
	}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrEvaluator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FSpectrQualifier Qualifier;

	UPROPERTY(EditAnywhere)
		FSpectrDecision Decision;

	mutable float Score;

	float Evaluate() const
	{
		Score = 0;
		float score = Qualifier.Qualify();
		Decision.SetScore(score);
		Score = score;
		return score;
	}
};

bool operator>(float Rhs, const FSpectrDecision& Lhs);
struct FTestContext
{
	bool bTreeInRange;
	bool bLogInRange;

	FTestContext()
		: bTreeInRange(true)
		, bLogInRange(false)
	{

	}
};

/*
	High level goal we try to create plan for.
*/
struct FSpectrGoal
{
	/*
		All of these tags must be present on Actor State to satisfy this goal.
	*/
	TMap<FName, bool> RequiredConditions;
	
	/* Qualify if planner should for a plan for this Goal. */
	FSpectrQualifier Qualifier;
};

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

	ASpectrAIController(const FObjectInitializer& ObjectInitializer);

	
};
