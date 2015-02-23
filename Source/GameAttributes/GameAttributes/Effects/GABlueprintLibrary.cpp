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

void UGABlueprintLibrary::ApplyEffect(const FHitResult& Target, APawn* Instigator, 
		UObject* Causer, FGAEffectSpec SpecIn)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.Actor.Get());
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return;

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FGAEffectContext context(Target.Location, Target.Actor, Causer,
		Instigator, targetComp, instiComp);

	SpecIn.Context = context;

	//SpecIn.GetModifiers();
	instiComp->ApplyEffectToTarget(SpecIn, context);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectActor(AActor* Target, APawn* Instigator,
	UObject* Causer, FGAEffectSpec SpecIn)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target);
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return FGAEffectHandle();

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FGAEffectContext context(Target->GetActorLocation(), Target, Causer,
		Instigator, targetComp, instiComp);

	SpecIn.Context = context;

	//SpecIn.GetModifiers();
	return instiComp->ApplyEffectToTarget(SpecIn, context);
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