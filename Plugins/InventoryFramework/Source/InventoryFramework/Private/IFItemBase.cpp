// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemBase.h"

#include "Engine/NetDriver.h"
#include "GameFramework/Actor.h"
#include "IFInventoryComponent.h"

bool UIFItemBase::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != NULL);

	UIFInventoryComponent* Owner = CastChecked<UIFInventoryComponent>(GetOuter());
	UNetDriver* NetDriver = Owner->GetOwner()->GetNetDriver();
	if (NetDriver)
	{
		NetDriver->ProcessRemoteFunction(Owner->GetOwner(), Function, Parameters, OutParms, Stack, this);
		return true;
	}

	return false;
}

