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
	if (OutgoingEffectComponent)
		OutgoingEffectComponent->OnEffectOutgoing.AddDynamic(this, &UGESEffect::OnOutgoingEffect);

	if (IncomingEffectComponent)
		IncomingEffectComponent->OnEffectIncoming.AddDynamic(this, &UGESEffect::OnIncomingEffect);

	if (OutgoingEffectComponent)
		OutgoingEffectComponent->OnEffectOutgoing.Broadcast(this);
	if (IncomingEffectComponent)
		IncomingEffectComponent->OnEffectIncoming.Broadcast(this);
}

bool UGESEffect::ModifyOutgoingEffect_Implementation(class UGESEffect* EffectIn)
{
	return false;
}

bool UGESEffect::ModifyIncomingEffect_Implementation(class UGESEffect* EffectIn)
{
	return false;
}

void UGESEffect::OnOutgoingEffect(class UGESEffect* EffectIn)
{
	ModifyOutgoingEffect(EffectIn);
}

void UGESEffect::OnIncomingEffect(class UGESEffect* EffectIn)
{
	ModifyIncomingEffect(EffectIn);
}