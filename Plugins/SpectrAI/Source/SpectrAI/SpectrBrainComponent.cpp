// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrBrainComponent.h"
#include "SpectrContext.h"

USpectrBrainComponent::USpectrBrainComponent()
{
	if(Context)
		CurrentContext = Cast<USpectrContext>(CreateDefaultSubobject(TEXT("CurrentContext"), Context, Context, true, false, false));
}

void USpectrBrainComponent::StarPlanning()
{
	TArray<TSubclassOf<USpectrAction>> OutActionList;
	SpectrAI.Plan(Goal, CurrentState, OutActionList, ActionList, CurrentContext);

	for (const TSubclassOf<USpectrAction>& Action : OutActionList)
	{
		FString name = Action.GetDefaultObject()->GetName();
		
		UE_LOG(LogTemp, Log, TEXT("Action Name: %s \n"), *name);
	}
}