// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAAttributeComponent.h"
#include "GAEffectExecution.h"

UGAEffectExecution::UGAEffectExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGAEffectExecution::ExecuteEffect(FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAExecutionContext& Context)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution class implementation"));

	Context.TargetAttributeComp->ModifyAttribute(ModIn);
}