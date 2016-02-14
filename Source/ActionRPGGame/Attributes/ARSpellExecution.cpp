// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "ARCharacterAttributes.h"
#include "ARSpellExecution.h"

#include "GAGameEffect.h"

UARSpellExecution::UARSpellExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UARSpellExecution::ExecuteEffect(FGAGameEffectHandle& HandleIn, 
	FGAEffectMod& ModIn, FGAExecutionContext& Context)
{
	UGAAttributeComponent* Instigator = Context.InstigatorAttributeComp;
	UGAAttributeComponent* Target = Context.TargetAttributeComp;

	float InstigatorMod = Instigator->GameEffectContainer.EffectModifiers.GetOutgoingMods(HandleIn.GetEffectSpec()->OwnedTags);
	float TargetMod = Target->GameEffectContainer.EffectModifiers.GetIncomingMods(HandleIn.GetEffectSpec()->OwnedTags);


	ModIn.Value = ModIn.Value + InstigatorMod - TargetMod;

	Context.TargetAttributeComp->ModifyAttribute(ModIn);
}