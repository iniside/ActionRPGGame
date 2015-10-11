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

FGAGameEffectHandle UGABlueprintLibrary::MakeOutgoingSpec(FGAGameEffectHandle Handle,
	TSubclassOf<class UGAGameEffectSpec> SpecIn, const FHitResult& Target, APawn* Instigator,
	UObject* Causer, EGAMakeSpecResult& ResultOut)
{
	if(!SpecIn)
	{
		ResultOut = EGAMakeSpecResult::Invalid;
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAGameEffectHandle();
	}
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.GetActor());
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
	{
		ResultOut = EGAMakeSpecResult::Invalid;
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Target or Instigator"));
		return FGAGameEffectHandle();
	}

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();

	FGAEffectContext Context(Target.Location, Target.GetActor(), Causer,
		Instigator, targetComp, instiComp);
	UE_LOG(GameAttributesEffects, Log, TEXT("Created new Context: %f"), *Context.ToString());

	if (Handle.IsValid())
	{
		Handle.SetContext(Context);
		UE_LOG(GameAttributesEffects, Log, TEXT("Set New Context for handle"));
		ResultOut = EGAMakeSpecResult::OldHandle;
		return Handle;
	}

	FGAGameEffectHandle NewHandle = instiComp->MakeGameEffect(SpecIn, Context);
	UE_LOG(GameAttributesEffects, Log, TEXT("Created New Handle"));
	ResultOut = EGAMakeSpecResult::NewHandle;
	return NewHandle;
}

void UGABlueprintLibrary::ApplyGameEffect(FGAGameEffectHandle Handle)
{
	if (!Handle.IsValid())
		return;

	FGAEffectContext& Context = Handle.GetContext();

	Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect());
}