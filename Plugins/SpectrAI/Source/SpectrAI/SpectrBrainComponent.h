// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "Queue.h"
#include "SpectrAction.h"
#include "SpectrBrainComponent.generated.h"

//Node representing single step in plan.
struct FSpectrNode
{
	int32 Cost;
	int32 RemainingCost;
	TWeakPtr<FSpectrNode> Parent;
	TMap<FGameplayTag, bool> State;
	TSubclassOf<USpectrAction> Action;
	TArray<TSharedPtr<FSpectrNode>> Children;
	FSpectrNode() {}
	FSpectrNode(const TMap<FGameplayTag, bool>& InState, int32 InCost, int32 InRemainingCost)
		: Cost(InCost)
		, RemainingCost(InRemainingCost)
		, State(InState)
	{}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrAI
{
	GENERATED_BODY()
public:
	//Precondition, List of action for this precondition;
	TMap<FGameplayTag, TArray<TSubclassOf<USpectrAction>> > ActionMap;

	void BuildGraph2(TArray<TSharedPtr<FSpectrNode>>& InNodes, const TArray<TSubclassOf<USpectrAction>>& ActionList, TSharedPtr<FSpectrNode> PathParent)
	{
		for (TSharedPtr<FSpectrNode> Node : InNodes)
		{
			for (TSubclassOf<USpectrAction> Action : ActionList)
			{
				if (CheckGoal(Action.GetDefaultObject()->Effects, Node->State))
				{
					TMap<FGameplayTag, bool> NewState = Action.GetDefaultObject()->PreConditions; // AddGoalChanges(Action.GetDefaultObject()->PreConditions, Node->State);
					TSharedPtr<FSpectrNode> Node2 = MakeShareable(new FSpectrNode(NewState, 0, 0));
					Node2->Action = Action;
					Node->Children.Add(Node2);
					
					if (PathParent.IsValid())
					{
						PathParent->Cost += Action.GetDefaultObject()->Cost;
					}
					else
					{
						//first iteration;
						PathParent = Node;
						PathParent->Cost += Action.GetDefaultObject()->Cost;
					}
					BuildGraph2(Node->Children, ActionList, PathParent);
					break;
				}
			}
		}
	}
	void BuildGraph(const TMap<FGameplayTag, bool>& InTargetGoal, const TMap<FGameplayTag, bool>& InCurrent,
		TArray<TSubclassOf<USpectrAction>>& ActionQueue, const TArray<TSubclassOf<USpectrAction>>& ActionList,
		class USpectrContext* InContext)
	{

		TArray<TSharedPtr<FSpectrNode>> OpenNodes;
		TArray<TSharedPtr<FSpectrNode>> ClosedNodes;

		TSharedPtr<FSpectrNode> Node = MakeShareable(new FSpectrNode(InTargetGoal, 0, 0));

		//OpenNodes.Push(Node);

		TArray<TSubclassOf<USpectrAction>> AvailableActions = ActionList;


		//first find all nodes which will fullfil intial goal
		for (const TSubclassOf<USpectrAction>& Action : ActionList)
		{
			if (CheckGoal(Action.GetDefaultObject()->Effects, InTargetGoal))
			{
				TMap<FGameplayTag, bool> NewState = Action.GetDefaultObject()->PreConditions;// AddGoalChanges(Action.GetDefaultObject()->PreConditions, Node->State);
				TSharedPtr<FSpectrNode> Node2 = MakeShareable(new FSpectrNode(NewState, 0, 0));
				Node2->Action = Action;
				OpenNodes.Add(Node2);

				//remove action which will fullfil initial goal state.
				AvailableActions.Remove(Action);
			}
		}
		BuildGraph2(OpenNodes, AvailableActions, nullptr);
		

		for (TSharedPtr<FSpectrNode> Node : OpenNodes)
		{

		}
		//TArray<TSubclassOf<USpectrAction>> AvailableActions;
		//AvailableActions.Append(ActionList);
		//TArray<TSubclassOf<USpectrAction>> UsedList;
		//bool bDone = false;
		//while (AvailableActions.Num() > 0)
		//{
		//	TSubclassOf<USpectrAction> Action = AvailableActions.Pop();
		//	UsedList.Push(Action);
		//	UE_LOG(LogTemp, Log, TEXT("-Build Plan - Action Name: %s \n"), *Action.GetDefaultObject()->GetName());
		//	if (Action.GetDefaultObject()->EvaluateCondition(InContext) 
		//		&& CheckGoal(Action.GetDefaultObject()->Effects, InTargetGoal))
		//	{
		//		TMap<FGameplayTag, bool> NewGoal = AddGoalChanges(InTargetGoal,
		//			Action.GetDefaultObject()->PreConditions);
		//		
		//		ActionQueue.Add(Action);
		//		/*if (CheckGoal(UpdatedGoalState, NewGoal))
		//		{
		//			return;
		//		}*/
		//		UsedList.Append(AvailableActions);
		//		UsedList.Remove(Action);
		//		if (!bDone)
		//		{
		//			bDone = true;
		//			BuildGraph(NewGoal, NewGoal, ActionQueue, UsedList, InContext);
		//			break;
		//		}
		//	}
		//}
	}
	 
	bool CheckGoal(const TMap<FGameplayTag, bool>& InEffects, const TMap<FGameplayTag, bool>& InGoal)
	{
		bool bAchieved = false;
		for (TPair<FGameplayTag, bool> Test : InGoal)
		{
			UE_LOG(LogTemp, Log, TEXT("---Build Plan - Check Current Key %s Value %d \n"), *Test.Key.ToString(), Test.Value);
		}
		
		for (TPair<FGameplayTag, bool> Test : InEffects)
		{
			if (InGoal.Contains(Test.Key))
			{
				if (InGoal[Test.Key] == Test.Value)
				{
					UE_LOG(LogTemp, Log, TEXT("----Build Plan - Check Passed Test %s Value %d \n"), *Test.Key.ToString(), Test.Value);
					bAchieved = true;
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("----Build Plan - Check Failed Test %s Value %d \n"), *Test.Key.ToString(), Test.Value);
					bAchieved = false;
					break; //all or nothing.
				}
			}
		}


		return bAchieved;
	}

	TMap<FGameplayTag, bool> UpdateState(const TMap<FGameplayTag, bool>& InEffects,
		const TMap<FGameplayTag, bool>& InCurrentState)
	{
		TMap<FGameplayTag, bool> NewState;
		NewState.Append(InCurrentState);
		for (TPair<FGameplayTag, bool> Effect : InEffects)
		{
			if (NewState.Contains(Effect.Key))
			{
				bool* dd = NewState.Find(Effect.Key);
				*dd = Effect.Value;
			}
		}
		return NewState;
	}

	TMap<FGameplayTag, bool> AddGoalChanges(const TMap<FGameplayTag, bool>& InCurrentGoal, 
		const TMap<FGameplayTag, bool>& InGoalChange)
	{
		TMap<FGameplayTag, bool> NewSet;

		NewSet.Append(InCurrentGoal);
		UE_LOG(LogTemp, Log, TEXT("---Build Plan - AddGoalChanges PRE START"));
		for (TPair<FGameplayTag, bool> Test : InCurrentGoal)
		{
			UE_LOG(LogTemp, Log, TEXT("---Build Plan - Key %s Value %d \n"), *Test.Key.ToString(), Test.Value);
		}
		UE_LOG(LogTemp, Log, TEXT("---Build Plan - AddGoalChanges PRE END"));
		for (TPair<FGameplayTag, bool> Change : InGoalChange)
		{
			if (NewSet.Contains(Change.Key))
			{
				/*bool* dd = NewSet.Find(Change.Key);
				*dd = Change.Value;*/
			}
			else
			{
				NewSet.Add(Change.Key, Change.Value);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("---Build Plan - AddGoalChanges POST START"));
		for (TPair<FGameplayTag, bool> Test : NewSet)
		{
			UE_LOG(LogTemp, Log, TEXT("---Build Plan - Key %s Value %d \n"), *Test.Key.ToString(), Test.Value);
		}
		UE_LOG(LogTemp, Log, TEXT("---Build Plan - AddGoalChanges POST END"));
		return NewSet;
	}
	void InitializeMap(const TArray<TSubclassOf<USpectrAction>>& ActionList)
	{
		for (const TSubclassOf<USpectrAction> Action : ActionList)
		{
			for (const TPair<FGameplayTag, bool>& Effect : Action.GetDefaultObject()->Effects)
			{
				TArray<TSubclassOf<USpectrAction>>& ActionArray = ActionMap.FindOrAdd(Effect.Key);
				ActionArray.Add(Action);
			}
			
		}
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
