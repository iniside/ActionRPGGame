// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffect.h"
#include "IGAEffect.h"
#include "GAEffectComponent.h"

void FGAActiveEffect::ActivateEffect()
{
	if (Effect)
	{
		if (Effect->EffectType == EGAEffectType::Periodic)
		{
			//FTimerDelegate del = FTimerDelegate::CreateUObject(Effect->TargetEffectComponent, &UGESEffectComponent::EffectDurationExpired, Handle);
			FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAActiveEffect::ExecuteEffectPeriod);
			Effect->Target.Actor->GetWorldTimerManager().SetTimer(DurationHandle, del, Effect->Duration, true, 0);
		}
	}
}
void FGAActiveEffect::FinishEffect()
{
	CurrentPerioCount = 0;
	if (Effect)
	{
		Effect->Target.Actor->GetWorldTimerManager().ClearTimer(DurationHandle);
	}
}

void FGAActiveEffect::ExecuteEffectPeriod()
{
	if (Effect)
	{
		CurrentPerioCount++;
		if (CurrentPerioCount < Effect->PeriodCount)
		{
			Effect->CallEffectPeriod();
		}
		else
		{
			Effect->TargetEffectComponent->EffectDurationExpired(Handle);
		}
	}
}

UGAEffectComponent::UGAEffectComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UGAEffectComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UGAEffectComponent::EffectDurationExpired(FGAEffectHandle HandleIn)
{
	FGAEffectContainer* effCon = ActiveEffectsContainerMap.Find(HandleIn.Tag);
	if (effCon)
	{
		FGAActiveEffect ef = effCon->ActiveEffectsMap.FindAndRemoveChecked(HandleIn.Handle);
		ef.FinishEffect();
		//FGESActiveEffect* actEff = effCon->ActiveEffectsMap.Find(HandleIn.Handle);
		//if (actEff)
		//{
		//	actEff->FinishEffect();
		//}
	}
}

void UGAEffectComponent::ReceiveEffect(class UGAEffect* EffectIn)
{

}
bool UGAEffectComponent::ApplyEffectToSelf(FGAEffectSpec& SpecIn)
{
	FName Tag = SpecIn.EffectTag.GetTagName();

	ActiveEffectsContainerMap.Contains(Tag);
	/*
		TODO:
		1. Figure out effects applying other effects!
		2. Figure out stacking, behaviour!
		3. Figure out on going effects modifications!
		The base magnitued is calculated in object, which is trying to apply effects
		But What if we want other Object to affect magnitude, or dynamically change
		magniutude, if paremetrs from which it was calulcated, changed ?
		4. But most of all, Figure out, effects, which modify other effects! (if that's needed actually,
		it all boils down to attributes, so effect actually doesn't need to listen for other effects,
		it only needs to know, about attribute changes, and react accordingly).
		The worst case scnarior, would effects, which would listen for effects, and react accordingly if the
		incoming effect have required tag.
	
	*/
	bool bEffectAppiled = false;
	//OnEffectIncoming.Broadcast(EffectIn);
	if (SpecIn.EffectClass.GetDefaultObject()->EffectType != EGAEffectType::Instant) //add only if effect is not instant. false for now since, I have not way to check that ;)
	{
		FName Tag = SpecIn.EffectTag.GetTagName();
		FName KeyIn = SpecIn.Causer->GetClass()->GetFName();

		if (ActiveEffectsContainerMap.Contains(Tag))
		{
			FGAEffectContainer* activeEffectsCont = ActiveEffectsContainerMap.Find(Tag);
			if (FGAActiveEffect* actEff = activeEffectsCont->ActiveEffectsMap.Find(KeyIn))
			{
				float lul = 0;
				bEffectAppiled = false;
				//check stacking rule.
			}
			else
			{
				FGAActiveEffect activeEffect;
				FGAActiveEffect& tempEff = activeEffectsCont->ActiveEffectsMap.Add(KeyIn, activeEffect);
				tempEff.Handle = FGAEffectHandle(KeyIn, Tag);
				tempEff.Effect = SpecIn.CreateEffect();
				tempEff.Effect->Initialize();
				tempEff.ActivateEffect();
				
				bEffectAppiled = true;
			}
		}
		else
		{
			FGAEffectContainer ActEffCont;
			FGAActiveEffect ActEff;
			FGAEffectContainer& tempEffCont = ActiveEffectsContainerMap.Add(Tag, ActEffCont);

			FGAActiveEffect& tempEff = tempEffCont.ActiveEffectsMap.Add(KeyIn, ActEff);
			tempEff.Handle = FGAEffectHandle(KeyIn, Tag);
			tempEff.Effect = SpecIn.CreateEffect();
			tempEff.Effect->Initialize();
			tempEff.ActivateEffect();

			bEffectAppiled = true;
		}
	}
	return bEffectAppiled;
}
bool UGAEffectComponent::ApplyEffectToTarget(FGAEffectSpec& SpecIn, AActor* TargetIn)
{
	//first we check for interface.
	IIGAEffect* effectInt = Cast<IIGAEffect>(TargetIn);
	if (!effectInt)
		return false;


	
	/*
		Give chance to effects on this component to modify this effect
		before it will be appiled to target.
	*/
	//OnEffectOutgoing.Broadcast(EffectIn);
	return effectInt->GetEffectComponent()->ApplyEffectToSelf(SpecIn);

//	EffectIn->TargetEffectComponent->ApplyEffectToSelf(EffectIn);
}

void UGAEffectComponent::RemoveEffectFromSelf(const FGameplayTagContainer& TagsIn, int32 ToRemove)
{
	int32 RemoveCounter = 0;
	for (auto It = TagsIn.CreateConstIterator(); It; ++It)
	{
		if (FGAEffectContainer* ActEffCont = ActiveEffectsContainerMap.Find(It->GetTagName()))
		{
			//whelp creating iterator on each iteration is kind of bad
			//but it allows to easily remove elements of array
			//without hitting invalid indexes..
			for (auto It = ActEffCont->ActiveEffectsMap.CreateIterator(); It; ++It)
			{
				if (RemoveCounter >= ToRemove)
					return;

				if (It->Value.Effect)
				{
					It->Value.Effect->DestroyEffect();
					It->Value.Effect = nullptr;
				}
				ActEffCont->ActiveEffectsMap.Remove(It.Key());
				RemoveCounter++;
			}
		}
	}
}

void UGAEffectComponent::RemoveEffectFromTarget(AActor* TargetIn, const FGameplayTagContainer& TagsIn, int32 ToRemove)
{
	IIGAEffect* effectInt = Cast<IIGAEffect>(TargetIn);
	if (!effectInt)
		return;

	effectInt->GetEffectComponent()->RemoveEffectFromSelf(TagsIn, ToRemove);
}