// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrAIController.h"

bool operator>(float Rhs, const FSpectrDecision& Lhs)
{
	return Rhs > Lhs.Score;
}


ASpectrAIController::ASpectrAIController()
{
	Goal.Add(TEXT("WoodCollected"), true);

	CurrentState.Add(TEXT("HasWood"), false);
	CurrentState.Add(TEXT("HasAxe"), false);
	CurrentState.Add(TEXT("WoodCollected"), false);
	FTestContext Context;
	FChopTreeAction ChopWood;
	ChopWood.Name = "ChopWood";
	ChopWood.CustomConditon = true;
	ChopWood.PreCondition.Add(TEXT("HasWood"), false);
	//ChopWood.PreCondition.Add(TEXT("WoodChopped"), false);
	ChopWood.PreCondition.Add(TEXT("HasAxe"), true);
	ChopWood.Effects.Add(TEXT("HasWood"), true);

	FPickLogAction PickLogs;
	PickLogs.Name = "PickLogs";
	PickLogs.CustomConditon = false;
	PickLogs.PreCondition.Add(TEXT("HasWood"), false);
	PickLogs.Effects.Add(TEXT("HasWood"), true);

	FSpectrAction DropLogs;
	DropLogs.Name = "DropLogs";
	DropLogs.CustomConditon = true;
	DropLogs.PreCondition.Add(TEXT("HasWood"), true);
	DropLogs.Effects.Add(TEXT("HasWood"), false);
	DropLogs.Effects.Add(TEXT("WoodCollected"), true);

	FSpectrAction PickItem_Axe;
	PickItem_Axe.CustomConditon = true;
	PickItem_Axe.Name = "PickItem_Axe";
	PickItem_Axe.PreCondition.Add(TEXT("HasAxe"), false);
	PickItem_Axe.Effects.Add(TEXT("HasAxe"), true);

	
	Spectr.ActionList.Add(MakeShareable(&PickLogs));
	Spectr.ActionList.Add(MakeShareable(&DropLogs));
	Spectr.ActionList.Add(MakeShareable(&ChopWood));
	Spectr.ActionList.Add(MakeShareable(&PickItem_Axe));

	TArray<FSpectrAction> ActionQueue;
	TMap<FName, bool> MergedState;
	MergedState.Append(CurrentState);
	MergedState.Append(Goal);
	Spectr.Plan(Goal, CurrentState, ActionQueue, Context);

	float Dupa2 = 0;
	if (Dupa2 > 0)
	{

	}
}