// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "../GAAttributeComponent.h"
#include "GAEffect.h"
#include "../IGAAttributes.h"
#include "GABlueprintLibrary.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


//bool UGABlueprintLibrary::ApplyEffect(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator,
//	const FGAEffectDuration& Duration, const FGameplayTag& EffectTag,
//	const FGAEffectPolicy& EffectPolicy, const FGAEffectAttributeSpec& AttributesIn, const FGAEffectModifierSpec& ModSpecIn)
//{
//	bool bEffectAppiled = false;
//	//if (EffectClass && Target.Actor.IsValid() && Instigator && Causer && AttributesIn.IsValid()
//	//	&& Duration.IsValid())
//	//{
//	//	IIGAEffect* targetInt = Cast<IIGAEffect>(Target.Actor.Get());
//	//	IIGAEffect* instigatorInt = Cast<IIGAEffect>(Instigator);
//	//	if (!targetInt || !instigatorInt)
//	//		return bEffectAppiled;
//
//	//	UGAEffectComponent* targetComp = targetInt->GetEffectComponent();
//	//	UGAEffectComponent* instigComp = instigatorInt->GetEffectComponent();
//	//	//effectOut = ConstructObject<UGESEffect>(EffectClass);
//	//	FGAEffectSpec EffSpec;
//	//	EffSpec.EffectClass = EffectClass;
//	//	EffSpec.Target = Target;
//	//	EffSpec.Causer = Causer;
//	//	EffSpec.Instigator = Instigator;
//	//	EffSpec.TargetEffectComp = targetComp;
//	//	EffSpec.InstigatorEffectComp = instigComp;
//	//	EffSpec.Duration = Duration;
//	//	EffSpec.EffectTag = EffectTag;
//	//	EffSpec.EffectPolicy = EffectPolicy;
//	//	EffSpec.ModifierSpec = ModSpecIn;
//	//	EffSpec.SetAttributeData(AttributesIn);
//	//	//AttributeMod
//
//	//	//EffSpec.AttributeMod = AttributeMod;
//	//	bEffectAppiled = instigComp->ApplyEffectToTarget(EffSpec, Target.Actor.Get());
//	//	return bEffectAppiled;
//	//}
//	return bEffectAppiled;
//}

bool UGABlueprintLibrary::ApplyInstantEffect(UObject* Causer, const FHitResult& Target, APawn* Instigator,
	FGAEffectInstant InstantSpec)
{
	bool bEffectAppiled = false;
	IIGAAttributes* targetInt = Cast<IIGAAttributes>(Target.Actor.Get());
	IIGAAttributes* instigatorInt = Cast<IIGAAttributes>(Instigator);
	if (!targetInt || !instigatorInt)
		return bEffectAppiled;
	
	UGAAttributeComponent* targetComp = targetInt->GetAttributeComponent();
	UGAAttributeComponent* instigComp = instigatorInt->GetAttributeComponent();
	InstantSpec.EffectContext.Causer = Causer;
	InstantSpec.EffectContext.Instigator = Instigator;
	InstantSpec.EffectContext.Target = Target;
	InstantSpec.EffectContext.TargetActor = Target.Actor;
	InstantSpec.EffectContext.InstigatorComp = instigComp;
	InstantSpec.EffectContext.TargetComp = targetComp;

	bEffectAppiled = instigComp->ApplyEffectToTarget(InstantSpec, targetComp);

	return false;
}
bool UGABlueprintLibrary::ApplyInstantEffectClass(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator)
{
	bool bEffectApplied = false;
	IIGAAttributes* targetInt = Cast<IIGAAttributes>(Target.Actor.Get());
	IIGAAttributes* instigatorInt = Cast<IIGAAttributes>(Instigator);
	if (!targetInt || !instigatorInt)
		return bEffectApplied;

	UGAAttributeComponent* targetComp = targetInt->GetAttributeComponent();
	UGAAttributeComponent* instigComp = instigatorInt->GetAttributeComponent();
	FGAEffectContext ctx;
	ctx.Causer = Causer;
	ctx.Instigator = Instigator;
	ctx.Target = Target;
	ctx.TargetActor = Target.Actor;
	ctx.InstigatorComp = instigComp;
	ctx.TargetComp = targetComp;

	bEffectApplied = true;
	instigComp->ApplyEffectToTarget(ctx, EffectClass);
	return bEffectApplied;
}

bool UGABlueprintLibrary::ApplyInstantEffectFromClass(const FGAEffectContext& Context, FGAEffectInstant InstantSpec)
{
	bool bEffectAppiled = false;
	InstantSpec.EffectContext = Context;
	bEffectAppiled = Context.InstigatorComp->ApplyEffectToTarget(InstantSpec, Context.TargetComp.Get());

	return false;
}
//bool UGABlueprintLibrary::ApplyPeriodicEffect(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator,
//	const FGAEffectDuration& Duration, const FGameplayTag& EffectTag, const FGAEffectPolicy& EffectPolicy,
//	const FGAEffectAttributeSpec& AttributesIn)
//{
//	return false;
//}

bool UGABlueprintLibrary::ApplyDurationEffect(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator,
	FGAEffectDuration DurationSpec)
{
	bool bEffectAppiled = false;
	IIGAAttributes* targetInt = Cast<IIGAAttributes>(Target.Actor.Get());
	IIGAAttributes* instigatorInt = Cast<IIGAAttributes>(Instigator);
	if (!targetInt || !instigatorInt)
		return bEffectAppiled;

	UGAAttributeComponent* targetComp = targetInt->GetAttributeComponent();
	UGAAttributeComponent* instigComp = instigatorInt->GetAttributeComponent();

	DurationSpec.EffectContext.Causer = Causer;
	DurationSpec.EffectContext.Instigator = Instigator;
	//InstantSpec.EffectClass = EffectClass;
	DurationSpec.EffectContext.Target = Target;
	DurationSpec.EffectContext.InstigatorComp = instigComp;
	DurationSpec.EffectContext.TargetComp = targetComp;

	bEffectAppiled = instigComp->ApplyEffectToTarget(DurationSpec, targetComp);
	return bEffectAppiled;
}

void UGABlueprintLibrary::RemoveEffects(AActor* CauserIn, AActor* TargetIn, const FGameplayTagContainer& EffectsTags, int32 EffectCount)
{
	//IIGAEffect* targetInt = Cast<IIGAEffect>(TargetIn);
	//IIGAEffect* causerInt = Cast<IIGAEffect>(CauserIn);
	//if (!targetInt && !causerInt)
	//	return;

	//causerInt->GetEffectComponent()->RemoveEffectFromTarget(TargetIn, EffectsTags, EffectCount);
}

bool UGABlueprintLibrary::HasAnyEffect(AActor* Target, const FGameplayTagContainer& Tags)
{
	return false;
}