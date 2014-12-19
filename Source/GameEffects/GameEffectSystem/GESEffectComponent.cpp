// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffect.h"
#include "GESEffectComponent.h"

UGESEffectComponent::UGESEffectComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}
void UGESEffectComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (StaticEffectClasses.Num() > 0)
	{
		for (TSubclassOf<UGESEffect> effect : StaticEffectClasses)
		{
			UGESEffect* effectOut = ConstructObject<UGESEffect>(effect, this);

			effectOut->Target = GetOwner();
			effectOut->Causer = GetOwner();
			effectOut->Instigator = GetOwner();
			effectOut->OutgoingEffectComponent = this;
			effectOut->IncomingEffectComponent = this;
			effectOut->Initialize();
			StaticEffects.Add(effectOut);
		}
	}
}
void UGESEffectComponent::AddPeriodicEffect(class UGESEffect* EffectIn)
{
	/*
		so we first broadcast event on component from effect originated. It
		will usually be Insigator;
	*/
	if (EffectIn->OutgoingEffectComponent)
		EffectIn->OutgoingEffectComponent->OnEffectOutgoing.Broadcast(EffectIn);
	/*
		And then on Incoming component, which will be usually component of Target.
	*/
	if (EffectIn->IncomingEffectComponent)
		EffectIn->IncomingEffectComponent->OnEffectIncoming.Broadcast(EffectIn);
	//OnEffectAppiled.Broadcast(EffectIn);
}