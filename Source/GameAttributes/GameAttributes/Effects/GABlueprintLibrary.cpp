// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "../GAAttributeComponent.h"
#include "GAEffect.h"
#include "GAEffectSpecification.h"
#include "GABlueprintLibrary.h"
#include "../IGAAttributes.h"
#include "GABlueprintLibrary.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
FGAEffectHandle UGABlueprintLibrary::ApplyEffectSpec(const FHitResult& Target, APawn* Instigator,
	UObject* Causer, TSubclassOf<class UGAEffectSpecification> SpecIn)
{
	FGAEffectHandle ReturnHandle;
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.Actor.Get());
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return ReturnHandle;

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();

	FGAEffectContext context(Target.Location, Target.Actor, Causer,
		Instigator, targetComp, instiComp);
	FName EffectName;
	if (SpecIn.GetDefaultObject()->Policy.Type != EGAEffectType::Instant)
	{
		if (SpecIn.GetDefaultObject()->EffectName.CustomName)
			EffectName = SpecIn.GetDefaultObject()->EffectName.EffectName;
		else
			EffectName = SpecIn->GetFName();// Causer->GetClass()->GetFName();
	}
	ReturnHandle = instiComp->ApplyEffectToTarget(SpecIn, context, EffectName);
	return ReturnHandle;
}
FGAEffectHandle UGABlueprintLibrary::ApplyEffectActorSpec(AActor* Target, APawn* Instigator,
	UObject* Causer, TSubclassOf<class UGAEffectSpecification> SpecIn)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target);
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return FGAEffectHandle();

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FGAEffectContext context(Target->GetActorLocation(), Target, Causer,
		Instigator, targetComp, instiComp);
	FName EffectName;
	if (SpecIn.GetDefaultObject()->Policy.Type != EGAEffectType::Instant)
	{
		if (SpecIn.GetDefaultObject()->EffectName.CustomName)
			EffectName = SpecIn.GetDefaultObject()->EffectName.EffectName;
		else
			EffectName = SpecIn->GetFName();// Causer->GetClass()->GetFName();
	}

	//SpecIn.GetModifiers();
	return instiComp->ApplyEffectToTarget(SpecIn, context, EffectName);
}

void UGABlueprintLibrary::OverrideEffectDuration(TSubclassOf<class UGAEffectSpecification> SpecIn)
{
}

void UGABlueprintLibrary::OverrideEffectModifiers(TSubclassOf<class UGAEffectSpecification> SpecIn,
	TArray<FGAAttributeModifier> ModifiersIn)
{
}
void UGABlueprintLibrary::OverrideEffectModifier(TSubclassOf<class UGAEffectSpecification> SpecIn,
	TArray<FGAModifierOverride> OverridesIn)
{

}