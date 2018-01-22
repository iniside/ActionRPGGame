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

	FSpectrAction ChopWood;
	ChopWood.Name = "ChopWood";
	ChopWood.PreCondition.Add(TEXT("HasWood"), false);
	ChopWood.PreCondition.Add(TEXT("WoodChopped"), false);
	ChopWood.PreCondition.Add(TEXT("HasAxe"), true);
	ChopWood.Effects.Add(TEXT("WoodChopped"), true);

	FSpectrAction PickLogs;
	PickLogs.Name = "PickLogs";
	PickLogs.PreCondition.Add(TEXT("HasWood"), false);
	PickLogs.Effects.Add(TEXT("HasWood"), true);
	PickLogs.Effects.Add(TEXT("WoodCollected"), false);

	FSpectrAction DropLogs;
	DropLogs.Name = "DropLogs";
	DropLogs.PreCondition.Add(TEXT("HasWood"), true);
	DropLogs.Effects.Add(TEXT("HasWood"), false);
	DropLogs.Effects.Add(TEXT("WoodCollected"), true);

	//FSpectrAction ChopWood;
	//ChopWood.Name = "ChopWood";
	//ChopWood.PreCondition.Add(TEXT("HasWood"), false);
	//ChopWood.PreCondition.Add(TEXT("HasAxe"), true);
	//ChopWood.Effects.Add(TEXT("HasWood"), true);


	FSpectrAction PickItem_Axe;
	PickItem_Axe.Name = "PickItem_Axe";
	PickItem_Axe.PreCondition.Add(TEXT("HasAxe"), false);
	PickItem_Axe.Effects.Add(TEXT("HasAxe"), true);

	Spectr.ActionList.Add(ChopWood);
	Spectr.ActionList.Add(PickItem_Axe);
	Spectr.ActionList.Add(PickLogs);
	Spectr.ActionList.Add(DropLogs);

	TArray<FSpectrAction> ActionQueue;
	Spectr.Plan(Goal, CurrentState, ActionQueue);

	float Dupa2 = 0;
	if (Dupa2 > 0)
	{

	}
}