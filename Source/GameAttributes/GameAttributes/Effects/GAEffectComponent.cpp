// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffect.h"
#include "IGAEffect.h"
#include "../IGAAttributes.h"
#include "../GAAttributeComponent.h"
#include "GAEffectComponent.h"

void FGAActiveEffect::ActivateEffect()
{
	OwningEffectComp = Spec.TargetEffectComp;
	
	if (Spec.InitializeEffectSpecs())
	{
		if (Spec.EffectPolicy.bInstanceEffect && Spec.EffectClass)
		{
			Effect = ConstructObject<UGAEffect>(Spec.EffectClass);
			Effect->Initialize();
		}
		if (Spec.EffectPolicy.EffectType == EGAEffectType::Instant)
		{
			OnEffectInitialize(); //that's all apply and forget.
		}
		else if (Spec.EffectPolicy.EffectType == EGAEffectType::Periodic)
		{
			float DurationMagnitude = Spec.GetDuration();
			float PeriodMagnitude = Spec.GetPeriod();
			PeriodCount = DurationMagnitude / PeriodMagnitude;

			FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAActiveEffect::ExecuteEffectPeriod);
			Spec.Instigator->GetWorldTimerManager().SetTimer(DurationHandle, del, PeriodMagnitude, true, 0);

		}
		else if (Spec.EffectPolicy.EffectType == EGAEffectType::Duration)
		{
			float DurationMagnitude = Spec.Duration.DurationMagnitude.GetCurrentMagnitude();
			FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAActiveEffect::ExecuteEffectDuration);
			Spec.Instigator->GetWorldTimerManager().SetTimer(DurationHandle, del, DurationMagnitude, false, 0);
		}
	}
	else
	{
		OwningEffectComp->EffectDurationExpired(Handle);
	}
}
void FGAActiveEffect::FinishEffect()
{
	//cleanup ?
	if (Effect.IsValid())
	{
		Effect.Reset();
	}
}
void FGAActiveEffect::OnEffectInitialize()
{
	Spec.ApplyInitialMods();
}
void FGAActiveEffect::OnEffectOngoing()
{
	Spec.ApplyPeriodicMods();
}
void FGAActiveEffect::OnEffectEnded()
{
	Spec.ApplyEndMods();
}

void FGAActiveEffect::ApplyAttributeMods()
{
	Spec.ApplyPeriodicMods();
}
void FGAActiveEffect::ExecuteEffectPeriod()
{
	CurrentPeriodCount++;
	if (CurrentPeriodCount > PeriodCount)
	{
		Spec.Instigator->GetWorldTimerManager().ClearTimer(DurationHandle);
		OwningEffectComp->EffectDurationExpired(Handle);
		CurrentPeriodCount = 0;
		return;
	}
	ApplyAttributeMods();
}
void FGAActiveEffect::ExecuteEffectDuration()
{

}
void FGAActiveEffect::RestoreAttributeValue()
{

}

void FGAActiveEffect::OnEffectAdded(FGAEffectSpec& EffectIn)
{

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
	if (SpecIn.EffectPolicy.EffectType == EGAEffectType::Instant)
	{
		//even if effect is instant we still want it to be modified by existing effects
		//and we should probabaly prefilter map, to find only effects which are relevelant (have required tags).

		for (auto It = ActiveEffectsContainerMap.CreateIterator(); It; ++It)
		{
			for (auto AIt = It->Value.ActiveEffectsMap.CreateIterator(); AIt; ++AIt)
			{
				AIt->Value.OnEffectAdded(SpecIn);
			}
		}
		//hm we shouldn't really make new effect.
		//we could just execute it from spec.
		FGAActiveEffect activeEffect;
		activeEffect.Spec = SpecIn;
		activeEffect.ActivateEffect();
		//return, we have nothing else to do. Instant effects are one off.
		return true;
	}
	bool bEffectAppiled = false;
	for (auto It = ActiveEffectsContainerMap.CreateIterator(); It; ++It)
	{
		for (auto AIt = It->Value.ActiveEffectsMap.CreateIterator(); AIt; ++AIt)
		{
			AIt->Value.OnEffectAdded(SpecIn);
		}
	}
	//OnEffectIncoming.Broadcast(EffectIn);
	if (SpecIn.EffectPolicy.EffectType != EGAEffectType::Instant) //add only if effect is not instant. false for now since, I have not way to check that ;)
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
				tempEff.Spec = SpecIn;
			//	tempEff.Effect = SpecIn.CreateEffect();
			//	tempEff.Effect->Initialize();
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
			tempEff.Spec = SpecIn;
			//tempEff.Effect = SpecIn.CreateEffect();
			//tempEff.Effect->Initialize();
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

				//if (It->Value.Effect)
				//{
				//	It->Value.Effect->DestroyEffect();
				//	It->Value.Effect = nullptr;
				//}
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