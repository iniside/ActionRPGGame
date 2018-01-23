// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "Queue.h"
#include "SpectrAction.h"
#include "SpectrBrainComponent.generated.h"

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrAI
{
	GENERATED_BODY()
public:
	//Precondition, List of action for this precondition;
	TMap<FGameplayTag, TArray<TSubclassOf<USpectrAction>> > ActionMap;

	void BuildGraph(const TMap<FGameplayTag, bool>& InTargetGoal, const TMap<FGameplayTag, bool>& InCurrent,
		TArray<TSubclassOf<USpectrAction>>& ActionQueue, const TArray<TSubclassOf<USpectrAction>>& ActionList,
		class USpectrContext* InContext)
	{
		bool bDone = false;
		for (const TSubclassOf<USpectrAction>& Action : ActionList)
		{
			if (Action.GetDefaultObject()->EvaluateCondition(InContext) && CheckGoal(Action.GetDefaultObject()->PreConditions, InCurrent))
			{
				TMap<FGameplayTag, bool> NewState = AddGoalChanges(InCurrent, Action.GetDefaultObject()->Effects);
				ActionQueue.Add(Action);
				if (CheckGoal(InTargetGoal, NewState))
				{
					return;
				}
				if (!bDone)
				{
					bDone = true;
					BuildGraph(InTargetGoal, NewState, ActionQueue, ActionList, InContext);
					break;
				}
			}
		}
	}

	bool CheckGoal(const TMap<FGameplayTag, bool>& InTest, const TMap<FGameplayTag, bool>& InCurrent)
	{
		bool bAchieved = false;

		for (TPair<FGameplayTag, bool> Test : InTest)
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

	TMap<FGameplayTag, bool> AddGoalChanges(const TMap<FGameplayTag, bool>& InCurrent, const TMap<FGameplayTag, bool>& InChange)
	{
		TMap<FGameplayTag, bool> NewSet;

		NewSet.Append(InCurrent);

		for (TPair<FGameplayTag, bool> Change : InChange)
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

	void Plan(const TMap<FGameplayTag, bool>& InTargetGoal, const TMap<FGameplayTag, bool>& InCurrentState,
		TArray<TSubclassOf<USpectrAction>>& InActionQueue, const TArray<TSubclassOf<USpectrAction>>& ActionList,
		class USpectrContext* InContext)
	{
		BuildGraph(InTargetGoal, InCurrentState, InActionQueue, ActionList, InContext);
	}
};

//USTRUCT(BlueprintType)
//struct SPECTRAI_API FSpectrSelector
//{
//	GENERATED_BODY()
//public:
//
//	UPROPERTY(EditAnywhere)
//		TArray<FSpectrEvaluator> Evaluators;
//
//	void Choose()
//	{
//		TArray<FSpectrDecision> SortedDecisions; //actually there will be only one highest scoring PoC
//		for (const FSpectrEvaluator& Evaluator : Evaluators)
//		{
//			if (SortedDecisions.Num() == 0)
//			{
//				Evaluator.Evaluate();
//				SortedDecisions.Add(Evaluator.Decision);
//			}
//			else
//			{
//				if (Evaluator.Evaluate() > SortedDecisions[0])
//				{
//					SortedDecisions[0] = Evaluator.Decision;
//				}
//			}
//		}
//	}
//};


/**
 * 
 */
UCLASS()
class SPECTRAI_API USpectrBrainComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, bool> Goal;
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, bool> CurrentState;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<USpectrAction>> ActionList;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class USpectrContext> Context;

	class USpectrContext* CurrentContext;

	FSpectrAI SpectrAI;

	USpectrBrainComponent();
	UFUNCTION(BlueprintCallable)
		void StarPlanning();
	
};
