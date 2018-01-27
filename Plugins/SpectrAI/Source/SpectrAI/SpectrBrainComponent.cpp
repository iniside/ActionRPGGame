// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrBrainComponent.h"
#include "SpectrContext.h"
#include "AIController.h"


bool operator==(const FSpectrNode& Other, const USpectrAction*& Action)
{
	return Other.Action == Action;
}

USpectrBrainComponent::USpectrBrainComponent()
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
		Actions.Add(Action);
	}
}
void USpectrBrainComponent::StarPlanning()
{
	TArray<USpectrAction*> OutActionList;
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		SpectrAI.Plan(Goal, CurrentState, OutActionList, Actions, CurrentContext, AIController);
	}
	for (int32 Idx = OutActionList.Num() - 1; Idx >= 0; Idx--)
	{
		FString name = OutActionList[Idx]->GetName();
		PendingPlan.Add(OutActionList[Idx]);
		UE_LOG(LogTemp, Log, TEXT("Action Name: %s \n"), *name);
	}
	ExecutePlan();
}

void USpectrBrainComponent::ExecutePlan()
{
	if (PendingPlan.Num() && PendingPlan[0])
	{
		CurrentAction = PendingPlan[0];
		PendingPlan.RemoveAt(0, 1, true);
	}
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		//Not in range
		if (!CurrentAction->NativeIsInRange())
		{
			FSMState = ESpectrState::Move;
			CurrentAction->NativeMoveTo(this);
			FSimpleDelegate Delegate = FSimpleDelegate::CreateUObject(CurrentAction, &USpectrAction::NativeOnMoveFinished, CurrentContext, AIController, this);
			PendingMoveEvent = Delegate;
		}
		else //in range, just execute action.
		{
			FSMState = ESpectrState::Action;
			CurrentAction->Execute(CurrentContext, AIController, this);
		}
		//
	}
}
void USpectrBrainComponent::OnActionFinished(USpectrAction* InAction)
{

}
void USpectrBrainComponent::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	PendingMoveEvent.ExecuteIfBound();
	//ExecutePlan();
}

void USpectrBrainComponent::MoveToLocation()
{
	FSMState = ESpectrState::Move;
}
void USpectrBrainComponent::MoveToActor(AActor* Target)
{
	FSMState = ESpectrState::Move;
	if (AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		AIController->MoveToActor(Target, 500);
	}
}