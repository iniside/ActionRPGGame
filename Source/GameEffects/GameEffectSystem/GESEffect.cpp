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
	if (OutgoingEffectComponent)
	{
		OutgoingEffectComponent->OnEffectOutgoing.AddDynamic(this, &UGESEffect::OnOutgoingEffect);
	}
	if (IncomingEffectComponent)
	{
		IncomingEffectComponent->OnEffectIncoming.AddDynamic(this, &UGESEffect::OnIncomingEffect);
		IncomingEffectComponent->OnCheckImmunity.BindUObject(this, &UGESEffect::CheckImmunity);
	}
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