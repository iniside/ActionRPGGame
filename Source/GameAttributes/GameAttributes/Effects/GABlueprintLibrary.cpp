// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "../GAAttributeComponent.h"


#include "GABlueprintLibrary.h"
#include "../IGAAttributes.h"
#include "GABlueprintLibrary.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

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

	Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle);
}