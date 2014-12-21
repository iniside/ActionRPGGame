// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffect.h"
#include "IGESEffect.h"
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
			effectOut->InstigatorEffectComponent = this;
			effectOut->TargetEffectComponent = this;
			effectOut->Initialize();
			
			for (auto It = effectOut->ImmunityTags.CreateConstIterator(); It; ++It)
			{
				if (FTagCounter* tagCount = ActiveImmunityTagsMap.Find(It->GetTagName()))
				{
					tagCount->TagCount += 1;
				}
				else
				{
					ActiveImmunityTagsMap.Add(It->GetTagName()).TagCount+=1;
				}
			}

			//for (auto It = effectOut->MyTags.CreateConstIterator(); It; ++It)
			//{
			//	if (FTagCounter* tagCount = ActiveEffectsTagsMap.Find(It->GetTagName()))
			//	{
			//		tagCount->TagCount += 1;
			//	}
			//	else
			//	{
			//		ActiveEffectsTagsMap.Add(It->GetTagName()).TagCount += 1;
			//	}
			//}
			
			StaticEffects.Add(effectOut);
		}
	}
}

void UGESEffectComponent::ReceiveEffect(class UGESEffect* EffectIn)
{
	OnEffectAppiled.Broadcast(EffectIn);
}
void UGESEffectComponent::ApplyEffectToSelf(class UGESEffect* EffectIn)
{
	//iterating over tags in container is probabaly not best idea
	//but it's still far better than others, and frankly I don't think there is going to be more than
	//few tags at worst in single container...
	//what if! You are immune to snare, but effect which is incoming deals fire damage and snare ?
	//should we discard entire effect, or just snare part ?
	//second choice seems more logical.
	//which would lead us to second point. One effect- one tag.
	//if effect apply multiple tags, it is because this effect will try to apply
	//other effects as well.
	for (auto It = EffectIn->MyTags.CreateConstIterator(); It; ++It)
	{
		if (ActiveImmunityTagsMap.Find(It->GetTagName()))
		{
			return; //we are immune to this effect. Return. Prolly give some callback to display
			//information on hud, or something.
		}
	}
	OnEffectIncoming.Broadcast(EffectIn);
	//we should probabaly call it before aggregating tags into maps.
	//if we get to this point, we are 100% sure that effect will be appiled to target
	//so there is no reason to delay it's execution by iterating over it's tags and aggregating them.

	//the optimal solution would be to move tag aggregation into seprate thread. and Queue task here from
	//upocming effects.
	EffectIn->OnEffectAppiled();

	if (false) //add only if effect is not instant. false for now since, I have not way to check that ;)
	{
		for (auto It = EffectIn->ImmunityTags.CreateConstIterator(); It; ++It)
		{
			if (FTagCounter* tagCount = ActiveImmunityTagsMap.Find(It->GetTagName()))
			{
				tagCount->TagCount += 1;
			}
			else
			{
				ActiveImmunityTagsMap.Add(It->GetTagName()).TagCount += 1;
			}
		}

		for (auto It = EffectIn->MyTags.CreateConstIterator(); It; ++It)
		{
			if (FTagCounter* tagCount = ActiveEffectsTagsMap.Find(It->GetTagName()))
			{
				tagCount->TagCount += 1;
			}
			else
			{
				ActiveEffectsTagsMap.Add(It->GetTagName()).TagCount += 1;
			}
		}
	}

}
void UGESEffectComponent::ApplyEffectToTarget(class UGESEffect* EffectIn, AActor* TargetIn)
{
	//first we check for interface.
	IIGESEffect* effectInt = Cast<IIGESEffect>(TargetIn);
	if (!effectInt)
		return;

	/*
		Give chance to effects on this component to modify this effect
		before it will be appiled to target.
	*/
	OnEffectOutgoing.Broadcast(EffectIn);
	effectInt->GetEffectComponent()->ApplyEffectToSelf(EffectIn);
//	EffectIn->TargetEffectComponent->ApplyEffectToSelf(EffectIn);
}


