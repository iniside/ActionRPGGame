// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "../GAGlobalTypes.h"
#include "../Attributes/GAAttributeBase.h"
#include "../GAAbilitiesComponent.h"
#include "GAEffectExecution.h"

UGAEffectExecution::UGAEffectExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UGAEffectExecution::PreModifyAttribute(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAEffectContext& Context)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution class implementation"));
}
void UGAEffectExecution::ExecuteEffect(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAEffectContext& Context)
{
	PreModifyAttribute(HandleIn, ModIn, Context);
	Context.TargetInterface->ModifyAttribute(ModIn, HandleIn);
}