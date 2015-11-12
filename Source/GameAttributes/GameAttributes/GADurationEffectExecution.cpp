// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GADurationEffectExecution.h"

UGADurationEffectExecution::UGADurationEffectExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGADurationEffectExecution::ExecuteEffect(FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAExecutionContext& Context)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution class implementation"));
}