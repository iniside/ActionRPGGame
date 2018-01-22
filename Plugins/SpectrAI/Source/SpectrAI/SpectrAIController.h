// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "Queue.h"
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

USTRUCT(BlueprintType)
struct FSpectrAction
{
	GENERATED_BODY()
public:
	FString Name;
	TMap<FName, bool> PreCondition;
	TMap<FName, bool> Effects;

	//precondition
	//Score to cosnider how viable given action will be
	UPROPERTY(EditAnywhere)
		FSpectrQualifier Qualifier;
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

USTRUCT(BlueprintType)
struct FSpectrPlanner
{
	GENERATED_BODY()
public:
	void Plan(const FSpectrAction& InAction, const FSpectrGoal& Goal)
	{
		TQueue < FSpectrAction, EQueueMode::Mpsc > ActionQueue;
	}
};

/*
Actions::
--ChopLog
	--Preconditions
		AI.HasTool
		AI.HasLogs
	--Effects
		AI.HasLogs


Actor:
	FSpectrGoal::
		--CollectWood
		Precondition:
			--AI.HasLogs
*/

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrAI
{
	GENERATED_BODY()
public:
	TArray<FSpectrAction> ActionList;

	//Precondition, List of action for this precondition;
	TMap<FGameplayTag, TArray<FSpectrAction>> ActionMap;

	void BuildGraph(const TMap<FName, bool>& InTargetGoal, const TMap<FName, bool>& InCurrent,
		TArray<FSpectrAction>& ActionQueue)
	{
		bool bDone = false;
		for (const FSpectrAction& Action : ActionList)
		{
			if (CheckGoal(Action.PreCondition, InCurrent))
			{
				TMap<FName, bool> NewState = AddGoalChanges(InCurrent, Action.Effects);
				ActionQueue.Add(Action);
				if (CheckGoal(InTargetGoal, NewState))
				{
					return;
				}
				if (!bDone)
				{
					bDone = true;
					BuildGraph(InTargetGoal, NewState, ActionQueue);
					break;
				}
			}
		}
	}

	bool CheckGoal(const TMap<FName, bool>& InTest, const TMap<FName, bool>& InCurrent)
	{
		bool bAchieved = false;
		
		for (TPair<FName, bool> Test : InTest)
		{
			if (InCurrent.Contains(Test.Key))
			{
				if (InCurrent[Test.Key] == Test.Value)
				{
					bAchieved = true;
				}
				else
				{
					bAchieved = false;
					break; //all or nothing.
				}
			}
		}


		return bAchieved;
	}

	TMap<FName, bool> AddGoalChanges(const TMap<FName, bool>& InCurrent, const TMap<FName, bool>& InChange)
	{
		TMap<FName, bool> NewSet;

		NewSet.Append(InCurrent);

		for (TPair<FName, bool> Change : InChange)
		{
			if (NewSet.Contains(Change.Key))
			{
				bool* dd = NewSet.Find(Change.Key);
				*dd = Change.Value;
			}
			else
			{
				NewSet.Add(Change.Key, Change.Value);
			}
		}

		return NewSet;
	}

	void Plan(const TMap<FName, bool>& InTargetGoal, const TMap<FName, bool>& InCurrentState, TArray<FSpectrAction>& InActionQueue)
	{
		BuildGraph(InTargetGoal, InCurrentState, InActionQueue);
	}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrSelector
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		TArray<FSpectrEvaluator> Evaluators;

	void Choose()
	{
		TArray<FSpectrDecision> SortedDecisions; //actually there will be only one highest scoring PoC
		for (const FSpectrEvaluator& Evaluator : Evaluators)
		{
			if (SortedDecisions.Num() == 0)
			{
				Evaluator.Evaluate();
				SortedDecisions.Add(Evaluator.Decision);
			}
			else
			{
				if (Evaluator.Evaluate() > SortedDecisions[0])
				{
					SortedDecisions[0] = Evaluator.Decision;
				}
			}
		}
	}
};

/*
	Current state of the worl from perspective of single AI Actor.
*/
UCLASS()
class SPECTRAI_API USpectrWorldState : public UObject
{
	GENERATED_BODY()
	FGameplayTagContainer ConditionStates;
};

/*
	Stores Context of world. Should be instanced per AI.
	Should be filled by external (or internal) changes like stimuly from senses,
	distance to interesting objects, or internal state of character (do I have this item in Inveotry ?).

*/

/**
 * 
 */
UCLASS()
class SPECTRAI_API ASpectrAIController : public AAIController
{
	GENERATED_BODY()
public:
	
	TMap<FName, bool> Goal;
	TMap<FName, bool> CurrentState;
	FSpectrAI Spectr;

	ASpectrAIController();
	UPROPERTY(EditAnywhere)
		FSpectrSelector Selector;
};
