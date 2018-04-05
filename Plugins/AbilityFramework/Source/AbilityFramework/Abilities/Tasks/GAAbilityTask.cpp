// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../../AbilityFramework.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask.h"

UGAAbilityTask::UGAAbilityTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsReplicated = false;
	SetFlags(RF_StrongRefOnFrame);
}
