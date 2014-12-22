// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffectComponent.h"
#include "GESEffect.h"

UGESEffect::UGESEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGESEffect::Initialize()
{
	/*
		Broadcast delegates, before binding functions in this event.
		To avoid self calling. This way only already existing effects, will call functions.
	*/
	//if (OutgoingEffectComponent)
	//	OutgoingEffectComponent->OnEffectOutgoing.Broadcast(this);
	//if (IncomingEffectComponent)
	//	IncomingEffectComponent->OnEffectIncoming.Broadcast(this);
	
	/*
		Instatn event don't need to listen for any effects, since they don't live long enough.
	*/
	if (EffectType == EGESEffectType::Infinite ||
		EffectType == EGESEffectType::Periodic)
	{
		if (InstigatorEffectComponent)
		{
			InstigatorEffectComponent->OnEffectOutgoing.AddDynamic(this, &UGESEffect::OnOutgoingEffect);
		}
		if (TargetEffectComponent)
		{
			TargetEffectComponent->OnEffectIncoming.AddDynamic(this, &UGESEffect::OnIncomingEffect);
			TargetEffectComponent->OnCheckImmunity.BindUObject(this, &UGESEffect::CheckImmunity);
		}
	}
	/*
		I assume that subsequent effects, will have same base data.
	*/
//	if (OtherEffects.Num() > 0)
//	{
//		for (UGESEffect* effect : OtherEffects)
//		{
	//		effect->Target = Target;
	//		effect->Instigator = Instigator;
	//		effect->Causer = Causer;
	//		effect->InstigatorEffectComponent = InstigatorEffectComponent;
	//		effect->TargetEffectComponent = TargetEffectComponent;
//			effect->Initialize();
//		}
//	}
	//if we call it here, we should give other effects chance, to modify this effect.
	//At least I hope so.
	OnEffectInitialized();
}

bool UGESEffect::ModifyOutgoingEffect_Implementation(class UGESEffect* EffectIn)
{
	return false;
}

bool UGESEffect::ModifyIncomingEffect_Implementation(class UGESEffect* EffectIn)
{
	return false;
}

bool UGESEffect::CheckImmunity(class UGESEffect* EffectIn)
{
	if (EffectIn->MyTags.MatchesAny(ImmunityTags, false))
	{
		return true;
	}
	return false;
}

void UGESEffect::CheckForImmunities(class UGESEffect* EffectIn)
{
	if (EffectIn->MyTags.MatchesAny(ImmunityTags, false))
	{
		//call another delegate ?
	}
}

void UGESEffect::OnOutgoingEffect(class UGESEffect* EffectIn)
{
	ModifyOutgoingEffect(EffectIn);
}

void UGESEffect::OnIncomingEffect(class UGESEffect* EffectIn)
{
	ModifyIncomingEffect(EffectIn);
}