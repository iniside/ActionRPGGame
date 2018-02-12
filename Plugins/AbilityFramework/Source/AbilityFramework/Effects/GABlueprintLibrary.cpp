// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "../AFAbilityComponent.h"


#include "GABlueprintLibrary.h"
#include "../AFAbilityInterface.h"
#include "GAEffectExtension.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToObject(UPARAM(ref) FGAEffectProperty& InEffect,
	class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToObject(InEffect, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToActor(UPARAM(ref) FGAEffectProperty& InEffect,
	class AActor* Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToActor(InEffect, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToLocation(UPARAM(ref) FGAEffectProperty& InEffect,
	const FHitResult& Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectFromHit(InEffect, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffect(FGAEffectProperty& InEffect,
	class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FHitResult& HitIn, const FAFFunctionModifier& Modifier )
{

	IAFAbilityInterface* Ability = Cast<IAFAbilityInterface>(Causer);
	if (!Ability)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect Effects must be applied trough Ability"));
		return FGAEffectHandle();
	}
	FGAEffectContext Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);
	InEffect.InitializeIfNotInitialized(Context);

	if (!InEffect.IsInitialized())
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAEffectHandle();
	}

	
	
	UAFEffectsComponent* Target2 = Context.GetTargetEffectsComponent();
	if (!Target2->HaveEffectRquiredTags(InEffect.GetSpec()->RequiredTags))
	{
		return FGAEffectHandle();
	}
	if (Target2->DenyEffectApplication(InEffect.GetSpec()->DenyTags))
	{
		return FGAEffectHandle();
	}
	UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.ToString());

	FGAEffect* effect = nullptr;
	if (InEffect.GetDuration() <= 0 && InEffect.GetPeriod() <= 0)
	{
		if (!InEffect.IsHandleValid(Target))
		{
			effect = new FGAEffect(InEffect.GetSpec(), Context);
			AddTagsToEffect(effect);
			effect->Context = Context;
			effect->GameEffect = InEffect.GetSpec();
		}
		else
		{
			effect = InEffect.GetHandle(Target).GetEffectPtr().Get();
		}
	}
	else
	{
		effect = new FGAEffect(InEffect.GetSpec(), Context);
		AddTagsToEffect(effect);
		effect->Context = Context;
		effect->GameEffect = InEffect.GetSpec();
	}
	if (Ability)
	{
		InEffect.SetPredictionHandle(Ability->GetPredictionHandle());
		if (effect)
		{
			effect->PredictionHandle = Ability->GetPredictionHandle();
		}
	}
	IAFAbilityInterface* InstigatorInterface = Cast<IAFAbilityInterface>(Instigator); 

	FGAEffectHandle Handle = InstigatorInterface->ApplyEffectToTarget(effect, InEffect, Context, Modifier);
	if (InEffect.GetDuration() > 0 || InEffect.GetPeriod() > 0)
	{
		InEffect.AddHandle(Context.Target.Get(), Handle);
	}
	
	return Handle;
}
FGAEffectHandle UGABlueprintLibrary::ApplyEffect(FGAEffectProperty* InEffect,
	class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FHitResult& HitIn, const FAFFunctionModifier& Modifier)
{
	IAFAbilityInterface* Ability = Cast<IAFAbilityInterface>(Causer);
	if (!Ability)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect Effects must be applied trough Ability"));
		return FGAEffectHandle();
	}
	FGAEffectContext Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);

	InEffect->InitializeIfNotInitialized(Context);

	if (!InEffect->IsInitialized())
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAEffectHandle();
	}

	
	
	/*if (!Context.IsValid())
	{
	return FGAEffectHandle();
	}*/
	UAFEffectsComponent* Target2 = Context.GetTargetEffectsComponent();
	if (!Target2->HaveEffectRquiredTags(InEffect->GetSpec()->RequiredTags))
	{
		return FGAEffectHandle();
	}
	if (Target2->DenyEffectApplication(InEffect->GetSpec()->DenyTags))
	{
		return FGAEffectHandle();
	}
	UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.ToString());

	FGAEffect* effect = nullptr;
	if (InEffect->GetDuration() <= 0 && InEffect->GetPeriod() <= 0)
	{
		if (!InEffect->IsHandleValid(Target))
		{
			effect = new FGAEffect(InEffect->GetSpec(), Context);
			AddTagsToEffect(effect);
			effect->Context = Context;
			effect->GameEffect = InEffect->GetSpec();
		}
		else
		{
			effect = InEffect->GetHandle(Target).GetEffectPtr().Get();
		}
	}
	else
	{
		effect = new FGAEffect(InEffect->GetSpec(), Context);
		AddTagsToEffect(effect);
		effect->Context = Context;
		effect->GameEffect = InEffect->GetSpec();
	}
	if (Ability)
	{
		InEffect->SetPredictionHandle(Ability->GetPredictionHandle());
		if (effect)
		{
			effect->PredictionHandle = Ability->GetPredictionHandle();
		}
	}
	IAFAbilityInterface* InstigatorInterface = Cast<IAFAbilityInterface>(Instigator);
	FGAEffectHandle Handle = InstigatorInterface->ApplyEffectToTarget(effect, *InEffect, Context, Modifier);
	if (InEffect->GetDuration() > 0 || InEffect->GetPeriod() > 0)
	{
		InEffect->AddHandle(Context.Target.Get(), Handle);
	}
	return Handle;
}
FGAEffectHandle UGABlueprintLibrary::ApplyEffectFromHit(FGAEffectProperty& InEffect,
	const FHitResult& Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	return ApplyEffect(InEffect, Target.GetActor(), Instigator, Causer, Target, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToActor(FGAEffectProperty& InEffect,
	class AActor* Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(InEffect, Target, Instigator, Causer, Hit, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToObject(FGAEffectProperty& InEffect,
	class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FAFFunctionModifier& Modifier)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(InEffect, Target, Instigator, Causer, Hit, Modifier);
}
FGAEffectHandle UGABlueprintLibrary::MakeEffect(UGAGameEffectSpec* SpecIn,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FHitResult& HitIn)
{
	if (!SpecIn)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAEffectHandle();
	}

	FGAEffectContext Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);
	if (!Context.IsValid())
	{
		//if the handle is valid (valid pointer to effect and id)
		//we want to preseve it and just set bad context.
		if (HandleIn.IsValid())
		{
			HandleIn.SetContext(Context);
			return HandleIn;
		}
		else
		{
			return FGAEffectHandle();
		}
	}

	UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.ToString());

	if (HandleIn.IsValid())
	{
		HandleIn.SetContext(Context);
	}
	else
	{
		FGAEffect* effect = new FGAEffect(SpecIn, Context);
		AddTagsToEffect(effect);
		HandleIn = FGAEffectHandle::GenerateHandle(effect);
		effect->Handle = HandleIn;
	}
	return HandleIn;
}

FGAEffectContext UGABlueprintLibrary::MakeContext(class UObject* Target, class APawn* Instigator, 
	AActor* InAvatar, UObject* Causer, const FHitResult& HitIn)
{
	IAFAbilityInterface* targetAttr = Cast<IAFAbilityInterface>(Target);
	IAFAbilityInterface* instiAttr = Cast<IAFAbilityInterface>(Instigator);
	if (!targetAttr && !instiAttr)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Target and Instigator does not implement IAFAbilityInterface interface"));
		return FGAEffectContext();
	}
	UAFAbilityComponent* targetComp = nullptr;
	UAFAbilityComponent* instiComp = nullptr;
	if (targetAttr)
	{
		targetComp = targetAttr->GetAbilityComp();
	}
	if (instiAttr)
	{
		instiComp = instiAttr->GetAbilityComp();
	}
	
	FVector location = targetComp ? targetComp->GetOwner()->GetActorLocation() : FVector(HitIn.ImpactPoint.X, HitIn.ImpactPoint.Y, HitIn.ImpactPoint.Z);
	FGAEffectContext Context(
		targetAttr ? targetAttr->GetAttributes() : nullptr,
		instiAttr ? instiAttr->GetAttributes() : nullptr,
		location, Target, Causer,
		Instigator, targetComp, instiComp,
		InAvatar);
	Context.HitResult = HitIn;
	return Context;
}

void UGABlueprintLibrary::AddTagsToEffect(FGAEffect* EffectIn)
{
	if (EffectIn)
	{
		EffectIn->OwnedTags.AppendTags(EffectIn->GameEffect->OwnedTags);
		EffectIn->ApplyTags.AppendTags(EffectIn->GameEffect->ApplyTags);
	}
}

FGAEffectContext& UGABlueprintLibrary::GetContext(const FGAEffectHandle& InHandle)
{
	return InHandle.GetContextRef();
}

UAFAbilityComponent* UGABlueprintLibrary::GetTargetComponent(const FGAEffectHandle& InHandle)
{
	return InHandle.GetContextRef().InstigatorComp.Get();
}

UAFAbilityComponent* UGABlueprintLibrary::GetInstigatorComponent(const FGAEffectHandle& InHandle)
{
	return InHandle.GetContextRef().TargetComp.Get();
}

void UGABlueprintLibrary::BroadcastEffectEvent(UObject* Target, FGameplayTag EventTag)
{
	IAFAbilityInterface* targetAttr = Cast<IAFAbilityInterface>(Target);
	if (!targetAttr)
		return;

	UAFAbilityComponent* TargetComp = targetAttr->GetAbilityComp();
	if (!TargetComp)
		return;

	//FAFEventData EventData;
	//TargetComp->NativeTriggerTagEvent(EventTag, EventData);
}