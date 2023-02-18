// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "GAGlobalTypes.h"
#include "Attributes/GAAttributeBase.h"
#include "AFAbilityComponent.h"
#include "Effects/GAGameEffect.h"
#include "AFAbilityInterface.h"
#include "GAEffectExecution.h"

UGAEffectExecution::UGAEffectExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UGAEffectExecution::PreModifyAttribute(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, const FGAEffectContext& Context)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution class implementation"));
}
void UGAEffectExecution::ExecuteEffect(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, 
	const FAFEffectParams& Params,
	const FAFFunctionModifier& Modifier)
{
	PreModifyAttribute(HandleIn, ModIn, Params.GetContext());
	const FGAEffectContext& Context = Params.GetContext();
	const_cast<FAFEffectParams&>(Params).GetContext().TargetInterface->ModifyAttribute(ModIn, HandleIn, Params.GetProperty(), Context);
}