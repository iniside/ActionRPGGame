// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFAbilityComponent.h"


#include "GABlueprintLibrary.h"

#include "AFAbilityInterface.h"
#include "GAGameEffect.h"
#include "AFEffectCustomApplication.h"

#include "GAEffectExtension.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToObject(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, FGAEffectHandle Handle
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToObject(InEffect, Handle, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToActor(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, FGAEffectHandle Handle
	, class AActor* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToActor(InEffect, Handle, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToLocation(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, FGAEffectHandle Handle
	, const FHitResult& Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectFromHit(InEffect, Handle, Target, Instigator, Causer, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffect(
	FAFPropertytHandle& InEffect
	, const FGAEffectHandle& Handle
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FHitResult& HitIn
	, const FAFFunctionModifier& Modifier)
{
	IAFAbilityInterface* Ability = Cast<IAFAbilityInterface>(Causer);
	if (!Ability)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect Effects must be applied trough Ability"));
		return FGAEffectHandle();
	}

	IAFAbilityInterface* TargetInterface = Cast<IAFAbilityInterface>(Target);
	if (!InEffect.GetClass().GetDefaultObject()->Application.GetDefaultObject()->CanApply(TargetInterface, InEffect.GetClass()))
	{
		return FGAEffectHandle();
	}
	bool bReusedParams = false;
	bool bPeriodicEffect = false;
	bool bReusedSpec = false;
	if (Handle.IsValid() && (InEffect.GetDuration() > 0 || InEffect.GetPeriod() > 0))
	{
		bPeriodicEffect = true;
	}

	FAFContextHandle Context;
	FAFEffectSpecHandle EffectSpecHandle;
	
	if (Handle.IsValid())
	{
		FAFContextHandle ExistingContext = InEffect.GetContext(Handle);
		if (ExistingContext.IsValid())
		{
			Context = ExistingContext;
			Context.SetTarget(Target);
			bReusedParams = true;
		}
		FAFEffectSpecHandle SpecHandle = InEffect.GetEffectSpec(Handle);
		EffectSpecHandle = SpecHandle;
		bReusedParams = true;
		bReusedSpec = true;
	}
	else
	{
		Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);
	}
	
	InEffect.InitializeIfNotInitialized(Context.GetRef());
	UAFEffectsComponent* Target2 = Context.GetRef().GetTargetEffectsComponent();
	

	if ((InEffect.GetDuration() > 0 || InEffect.GetPeriod() > 0))
	{
		bPeriodicEffect = true;
	}

	if (!InEffect.IsInitialized())
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAEffectHandle();
	}
		
	
	if (!Target2->HaveEffectRquiredTags(InEffect.GetSpec()->RequiredTags))
	{
		return FGAEffectHandle();
	}
	if (Target2->DenyEffectApplication(InEffect.GetSpec()->DenyTags))
	{
		return FGAEffectHandle();
	}
	UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.GetRef().ToString());

	FGAEffect effect;
	effect.World = Instigator->GetWorld();
	if (Ability)
	{
		//InEffect.SetPredictionHandle(Ability->GetPredictionHandle());
		
		effect.PredictionHandle = Ability->GetPredictionHandle();
	}
	IAFAbilityInterface* InstigatorInterface = Cast<IAFAbilityInterface>(Instigator); 
	
	FAFEffectParams Params(InEffect);
	
	if (!bReusedSpec)
	{
		FAFEffectSpec* EffectSpec = new FAFEffectSpec(Context, InEffect.GetClass());
		AddTagsToEffect(EffectSpec);
		Params.EffectSpec = FAFEffectSpecHandle::Generate(EffectSpec);
	}
	else
	{
		Params.EffectSpec = EffectSpecHandle;
	}
	Params.bRecreated = bReusedParams;
	Params.bPeriodicEffect = bPeriodicEffect;
	Params.Context = Context;

	FGAEffectHandle NewHandle = InstigatorInterface->ApplyEffectToTarget(effect, Handle, Params, Modifier);
	
	return NewHandle;
}
FGAEffectHandle UGABlueprintLibrary::ApplyEffect(
	FGAEffectProperty* InEffect
	, const FGAEffectHandle& Handle
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FHitResult& HitIn
	, const FAFFunctionModifier& Modifier)
{
	//IAFAbilityInterface* Ability = Cast<IAFAbilityInterface>(Causer);
	//if (!Ability)
	//{
	//	UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect Effects must be applied trough Ability"));
	//	return FGAEffectHandle();
	//}
	//bool bReusedParams = false;
	//bool bPeriodicEffect = false;
	//bool bReusedSpec = false;
	//if (Handle.IsValid() && (InEffect->GetDuration() > 0 || InEffect->GetPeriod() > 0))
	//{
	//	bPeriodicEffect = true;
	//}

	//FAFContextHandle Context;
	//FAFEffectSpecHandle EffectSpecHandle;

	//if (Handle.IsValid())
	//{
	//	if (bPeriodicEffect)
	//	{
	//		FAFContextHandle* ExistingContext = InEffect->GetContext(Handle);
	//		if (ExistingContext)
	//		{
	//			Context = *ExistingContext;
	//			Context.SetTarget(Target);
	//			bReusedParams = true;
	//		}
	//		FAFEffectSpecHandle* SpecHandle = InEffect->GetEffectSpec(Handle);
	//		EffectSpecHandle = *SpecHandle;
	//		bReusedSpec = true;
	//	}
	//	else
	//	{
	//		Context = InEffect->GetInstantContext();
	//		Context.SetTarget(Target);

	//		EffectSpecHandle = InEffect->GetInstantEffectSpec();
	//		bReusedParams = true;
	//		bReusedSpec = true;
	//	}
	//}
	//else
	//{
	//	Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);
	//}

	//InEffect->InitializeIfNotInitialized(Context.GetRef());

	//if (!InEffect->IsInitialized())
	//{
	//	UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
	//	return FGAEffectHandle();
	//}

	//UAFEffectsComponent* Target2 = Context.GetRef().GetTargetEffectsComponent();
	//if (!Target2->HaveEffectRquiredTags(InEffect->GetSpec()->RequiredTags))
	//{
	//	return FGAEffectHandle();
	//}
	//if (Target2->DenyEffectApplication(InEffect->GetSpec()->DenyTags))
	//{
	//	return FGAEffectHandle();
	//}
	//UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.GetRef().ToString());

	//FGAEffect effect;
	//effect.World = Instigator->GetWorld();
	//if (Ability)
	//{
	//	InEffect->SetPredictionHandle(Ability->GetPredictionHandle());
	//	effect.PredictionHandle = Ability->GetPredictionHandle();
	//	
	//}
	//
	//FAFPropertytHandle Property = FAFPropertytHandle::Generate(*InEffect);
	//FAFEffectParams Params(Property);
	//FAFEffectSpec* EffectSpec = new FAFEffectSpec();
	////AddTagsToEffect(EffectSpec);
	//Params.Context = Context;
	//
	//Params.EffectSpec = FAFEffectSpecHandle::Generate(EffectSpec);

	//IAFAbilityInterface* InstigatorInterface = Cast<IAFAbilityInterface>(Instigator);
	//FGAEffectHandle NewHandle = InstigatorInterface->ApplyEffectToTarget(effect, Handle, Params, Modifier);
	FGAEffectHandle NewHandle;
	return NewHandle;
}
FGAEffectHandle UGABlueprintLibrary::ApplyEffectFromHit(
	FAFPropertytHandle& InEffect
	, const FGAEffectHandle& Handle
	, const FHitResult& Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffect(InEffect, Handle, Target.GetActor(), Instigator, Causer, Target, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToActor(
	FAFPropertytHandle& InEffect
	, const FGAEffectHandle& Handle
	, class AActor* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(InEffect, Handle, Target, Instigator, Causer, Hit, Modifier);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToObject(
	FAFPropertytHandle& InEffect
	, const FGAEffectHandle& Handle
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(InEffect, Handle, Target, Instigator, Causer, Hit, Modifier);
}

FAFContextHandle UGABlueprintLibrary::MakeContext(class UObject* Target, class APawn* Instigator,
	AActor* InAvatar, UObject* Causer, const FHitResult& HitIn)
{
	IAFAbilityInterface* targetAttr = Cast<IAFAbilityInterface>(Target);
	IAFAbilityInterface* instiAttr = Cast<IAFAbilityInterface>(Instigator);
	if (!targetAttr && !instiAttr)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Target and Instigator does not implement IAFAbilityInterface interface"));
		return FAFContextHandle();
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
	FGAEffectContext* Context = new FGAEffectContext(
		targetAttr ? targetAttr->GetAttributes() : nullptr
		, instiAttr ? instiAttr->GetAttributes() : nullptr
		, location
		, Target
		, Causer
		, Instigator
		, targetComp
		, instiComp
		, InAvatar);

	Context->HitResult = HitIn;
	
	FAFContextHandle Handle = FAFContextHandle::Generate(Context);

	return Handle;
}

void UGABlueprintLibrary::AddTagsToEffect(FAFEffectSpec* EffectIn)
{
	if (EffectIn)
	{
		EffectIn->AddOwnedTags(EffectIn->GetSpec()->OwnedTags);
		EffectIn->AddApplyTags(EffectIn->GetSpec()->ApplyTags);
	}
}

FGAEffectContext& UGABlueprintLibrary::GetContext(const FAFContextHandle& InHandle)
{
	return InHandle.GetRef();
}

UAFAbilityComponent* UGABlueprintLibrary::GetTargetComponent(const FGAEffectHandle& InHandle)
{
	return nullptr;// InHandle.GetContextRef().InstigatorComp.Get();
}

UAFAbilityComponent* UGABlueprintLibrary::GetInstigatorComponent(const FGAEffectHandle& InHandle)
{
	return nullptr;// InHandle.GetContextRef().TargetComp.Get();
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