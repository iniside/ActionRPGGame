// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrBrainComponent.h"
#include "SpectrContext.h"
#include "AIController.h"
#include "Engine/World.h"
#include "TimerManager.h"

bool operator==(const FSpectrNode& Other, const USpectrAction*& Action)
{
	return Other.Action == Action;
}

USpectrBrainComponent::USpectrBrainComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FSMState = ESpectrState::Idle;
	if(Context)
		CurrentContext = Cast<USpectrContext>(CreateDefaultSubobject(TEXT("CurrentContext"), Context, Context, true, false, false));
}

void USpectrBrainComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//check for new goals
	//evaluate if new goal is worth pursuing over the current one.
}
void USpectrBrainComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &USpectrBrainComponent::OnMoveFinished);
	}
	for (const TSubclassOf<USpectrAction>& ActionClass : ActionList)
	{
		USpectrAction* Action = NewObject<USpectrAction>(this, ActionClass);
		Action->OwningBrain = this;
		Actions.Add(Action);
	}
}
void USpectrBrainComponent::StarPlanning()
{
	//no point of running planner on clients.
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	TArray<USpectrAction*> OutActionList;
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		SpectrAI.Plan(Goal, CurrentState, OutActionList, Actions, CurrentContext, AIController);
	}
	for (int32 Idx = OutActionList.Num() - 1; Idx >= 0; Idx--)
	{
		FString name = OutActionList[Idx]->GetName();
		
		PendingPlan.Add(OutActionList[Idx]);
		PendingPlan2.Enqueue(OutActionList[Idx]);

		UE_LOG(LogTemp, Log, TEXT("Action Name: %s \n"), *name);
	}
	if (OutActionList.Num() == 0)
	{
		FTimerManager& Timer = GetWorld()->GetTimerManager();
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, &USpectrBrainComponent::NextPlan);
		Timer.SetTimer(NextPlanTimerHandle, del, 0.2f, false, 0.2f);
	}
	else
	{
		ExecutePlan(nullptr);
	}
	
}
void USpectrBrainComponent::NextPlan()
{
	TArray<USpectrAction*> OutActionList;
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		SpectrAI.Plan(Goal, CurrentState, OutActionList, Actions, CurrentContext, AIController);
	}
	if (OutActionList.Num() == 0)
	{
		FTimerManager& Timer = GetWorld()->GetTimerManager();
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, &USpectrBrainComponent::NextPlan);
		Timer.SetTimer(NextPlanTimerHandle, del, 0.2f, false, 0.2f);
	}
	else
	{
		ExecutePlan(nullptr);
	}
	
}
void USpectrBrainComponent::SelectGoal()
{

}

void USpectrBrainComponent::ExecutePlan(class USpectrAction* PreviousAction)
{
	if (PendingPlan2.IsEmpty())
	{
		//plan finished
		StarPlanning(); //plan again
		//in reality look for new goal.
		return;
	}
	while (!PendingPlan2.IsEmpty())
	{
		USpectrAction* OutAction;
		PendingPlan2.Dequeue(OutAction);
		if (OutAction != PreviousAction)
		{
			CurrentAction = OutAction;
			break;
		}
	}
	/*if (PendingPlan.Num() && PendingPlan[0])
	{
		CurrentAction = PendingPlan[0];
		PendingPlan.RemoveAt(0, 1, true);
	}*/
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		//Not in range
		if (!CurrentAction->NativeIsInRange(AIController))
		{
			FSMState = ESpectrState::Move;
			CurrentAction->NativeMoveTo(this);
			FSimpleDelegate Delegate = FSimpleDelegate::CreateUObject(CurrentAction, &USpectrAction::NativeOnMoveFinished, CurrentContext, AIController, this);
			PendingMoveEvent = Delegate;
		}
		else //in range, just execute action.
		{
			FSMState = ESpectrState::Action;
			CurrentAction->NativeExecute(CurrentContext, AIController, this);
		}
		//
	}
}
void USpectrBrainComponent::AbortPlan(const FString& Reason)
{

}
void USpectrBrainComponent::OnActionFinished(USpectrAction* InAction)
{
	ExecutePlan(InAction);
}
void USpectrBrainComponent::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (CurrentAction)
		{
			CurrentAction->NativeOnMoveFinished(CurrentContext, AIController, this);
		}
	}
	//PendingMoveEvent.ExecuteIfBound();
	//PendingMoveEvent.Unbind();
	//ExecutePlan();
}

void USpectrBrainComponent::MoveToLocation()
{
	FSMState = ESpectrState::Move;
}
void USpectrBrainComponent::MoveToActor(AActor* Target, float MinDistance)
{
	FSMState = ESpectrState::Move;
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		AIController->MoveToActor(Target, MinDistance);
	}
}