// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTasksComponent.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "Queue.h"
#include "SpectrAction.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"

#include "SpectrBrainComponent.generated.h"

enum class ESpectrState : uint8
{
	Idle = 0,
	Action = 1,
	Move = 2
};

//Node representing single step in plan.
struct FSpectrNode
{
	int32 Cost;
	float Score;
	int32 RemainingCost;
	TWeakPtr<FSpectrNode> Parent;
	TMap<FGameplayTag, bool> State;
	USpectrAction* Action;
	TArray<TSharedPtr<FSpectrNode>> Children;
	FSpectrNode() {}
	FSpectrNode(const TMap<FGameplayTag, bool>& InState, int32 InCost, int32 InRemainingCost)
		: Cost(InCost)
		, RemainingCost(InRemainingCost)
		, State(InState)
	{}

	bool Equals(USpectrAction* InAction)
	{
		return Action == InAction;
	}

	bool operator==(const FSpectrNode& Other) const
	{
		return Action == Other.Action;
	}
	bool operator==(const USpectrAction*& Other) const
	{
		return Action == Other;
	}
	
};

bool operator==(const FSpectrNode& Other, const USpectrAction*& Action);


USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrAI
{
	GENERATED_BODY()
public:
	//Precondition, List of action for this precondition;
	TMap<FGameplayTag, TArray<TSubclassOf<USpectrAction>> > ActionMap;

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
	void Plan(const TMap<FGameplayTag, bool>& InTargetGoal, const TMap<FGameplayTag, bool>& InCurrentState
		, TArray<class USpectrAction*>& InActionQueue
		, const TArray<class USpectrAction*>& ActionList
		, class USpectrContext* InContext
		, class AAIController* AIController)
	{
		TArray<TSharedPtr<FSpectrNode>> OpenNodes;
		TArray<TSharedPtr<FSpectrNode>> ClosedNodes;

		TSharedPtr<FSpectrNode> Node = MakeShareable(new FSpectrNode(InTargetGoal, 0, 0));

		//actually made action when constructing node path.
		//and then execute it in reverse (from last item in array to first).
		//so we don't was time traversing node tree and then reordering actions..
		TArray<class USpectrAction*> ActionPlan;

		OpenNodes.Push(Node);

		TArray<class USpectrAction*> AvailableActions = ActionList;

		//reverse A*, or something similiar to it at least..
		while (OpenNodes.Num())
		{
			TSharedPtr<FSpectrNode> CurrentNode = OpenNodes.Pop();
			ClosedNodes.Push(CurrentNode);

			for (USpectrAction*& Action : AvailableActions)
			{
				//or instead of pushing to closed actions... we can remove them for Available Actions hmm ?
				if (ClosedNodes.ContainsByPredicate([&](const TSharedPtr<FSpectrNode>& Item)
				{
					return Item->Equals(Action);
				}))
				{
					continue;
				}

				if (!(CurrentNode->Action == Action))
				{
					if (CheckGoal(Action->Effects, CurrentNode->State))
					{
						//action scored Zero, so it probabaly can't be used anyway.
						if (!Action->NativeEvaluateCondition(InContext, AIController))
						{
							continue;
						}
						float Score = Action->NativeScore(InContext, AIController);

						TSharedPtr<FSpectrNode> Node2 = MakeShareable(new FSpectrNode(Action->PreConditions, Action->Cost, 0));
						Node2->Action = Action;
						Node2->Score = Score;
						OpenNodes.Add(Node2);

						if (CurrentNode->Children.Num() == 0)
						{
							CurrentNode->Children.Add(Node2);
							ActionPlan.Add(Action);
						}
						else
						{
							//add new action, only if it is cheaper than current one. 
							//very primitive we need to check if the actuall path will be cheaper.
							//so compare current cost of the path with
							if (Score > Node2->Score)
							{
								ActionPlan.Remove(CurrentNode->Children[0]->Action);
								CurrentNode->Children.Empty();
								CurrentNode->Children.Add(Node2);

								ActionPlan.Add(Action);
							}
							else
							{
								//action has been discarded, remove it from OpenNodes
								OpenNodes.Remove(Node2);
								//and add to closed nodes. No reason to ever consider it again.
								ClosedNodes.Add(Node2);
							}
						}
					}
				}
			}
		}

		InActionQueue = ActionPlan;
	}
};

USTRUCT(BlueprintType)
struct FGoalSpec
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced)
		class USpectrGoal* Goal;
	UPROPERTY(EditAnywhere, Instanced)
		TArray<class USpectrConsideration*> Considerations;
};

/**
 * 
 */
UCLASS()
class SPECTRAI_API USpectrBrainComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()
public:
	ESpectrState FSMState;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, bool> Goal;
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, bool> CurrentState;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<USpectrAction>> ActionList;
	UPROPERTY()
		TArray<class USpectrAction*> Actions;
	UPROPERTY(EditAnywhere)
		TArray<FGoalSpec> Goals2;

	UPROPERTY(EditAnywhere, Instanced)
		TArray<class USpectrGoal*> Goals;
	UPROPERTY()
		class USpectrGoal* CurrentGoal;


	UPROPERTY()
		TArray<USpectrAction*> PendingPlan;

	TQueue<USpectrAction*> PendingPlan2;

	UPROPERTY()
		USpectrAction* CurrentAction;



	UPROPERTY(EditAnywhere)
		TSubclassOf<class USpectrContext> Context;
	UPROPERTY()
		class USpectrContext* CurrentContext;

	FSpectrAI SpectrAI;

	//Map of pending move events.
	FSimpleDelegate PendingMoveEvent;

	FTimerHandle NextPlanTimerHandle;

	USpectrBrainComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
		void StarPlanning();
	UFUNCTION()
		void NextPlan();

	void SelectGoal();
	void ExecutePlan(class USpectrAction* PreviousAction);
	void AbortPlan(const FString& Reason);
	void OnActionFinished(USpectrAction* InAction);
	void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);

	void MoveToLocation();
	void MoveToActor(AActor* Target, float MinDistance);

	
};
