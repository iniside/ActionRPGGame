// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFAbilityComponent.h"


#include "GABlueprintLibrary.h"

#include "AFAbilityInterface.h"
#include "GAGameEffect.h"
#include "AFEffectCustomApplication.h"

#include "GAEffectExtension.h"
#include "Engine/Engine.h"
#include "AFSimpleInterface.h"
#include "AFAttributeInterface.h"

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

FString NetModeToString(ENetMode NetMode)
{
	switch (NetMode)
	{
	case ENetMode::NM_Client:
		return FString("Client");
	case ENetMode::NM_DedicatedServer:
		return FString("Server");
	case ENetMode::NM_ListenServer:
		return FString("ListenServer");
	case ENetMode::NM_Standalone:
		return FString("Standalone");
	}

	return FString("Invalid");
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
	ENetMode NetMode = GEngine->GetNetMode(Instigator->GetWorld());

	UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect START, NetMode %s"), *NetModeToString(NetMode));
	UE_LOG(GameAttributesEffects, Error, TEXT("-----"));
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
	
	//only reused Spec and Context if effect is instant ?
	if (Handle.IsValid() && !bPeriodicEffect)
	{
		Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);

		FAFContextHandle ExistingContext = InEffect.GetContext(Handle);
		/*if (ExistingContext.IsValid())
		{
			Context = ExistingContext;
			Context.SetTarget(Target);
			bReusedParams = true;
		}*/
		/*FAFEffectSpecHandle SpecHandle = InEffect.GetEffectSpec(Handle);
		EffectSpecHandle = SpecHandle;
		bReusedParams = true;
		bReusedSpec = true;*/
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
	if (Params.EffectSpec.GetPtr())
	{
		if (Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target.IsValid())
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("  Pre Spec Target: %s"), *Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target->GetName());
		}
	}
	if (!bReusedSpec)
	{
		FAFEffectSpec* EffectSpec = new FAFEffectSpec(Context, InEffect.GetClass());
		AddTagsToEffect(EffectSpec);
		Params.EffectSpec = FAFEffectSpecHandle::Generate(EffectSpec);
		if (EffectSpec->GetContext().GetPtr()->Target.IsValid())
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("  Pre Spec Target: %s"), *EffectSpec->GetContext().GetPtr()->Target->GetName());
		}
	}
	else
	{
		Params.EffectSpec = EffectSpecHandle;
		Params.EffectSpec.SetContext(Context);
	}
	if (Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target.IsValid())
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("  Post Spec Target: %s"), *Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target->GetName());
	}
	Params.bRecreated = bReusedParams;
	Params.bPeriodicEffect = bPeriodicEffect;
	Params.Context = Context;

	FGAEffectHandle NewHandle = InstigatorInterface->ApplyEffectToTarget(effect, Handle, Params, Modifier);

	UE_LOG(GameAttributesEffects, Error, TEXT("-----"));
	UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect END"));
	

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
void UGABlueprintLibrary::CreateEffectSpec(UPARAM(Ref) FAFEffectSpecHandle& InOutSpec
	, const FAFPropertytHandle& InEffect
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer)
{
	IAFAbilityInterface* TargetInterface = Cast<IAFAbilityInterface>(Target);
	
	FHitResult HitIn(ForceInit);
	FAFContextHandle Context;
	FAFEffectSpecHandle EffectSpecHandle;
	Context = MakeContext(Target, Instigator, nullptr, Causer, HitIn);
	
	UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Context.GetRef().ToString());

	FAFEffectSpec* EffectSpec = new FAFEffectSpec(Context, InEffect.GetClass());
	AddTagsToEffect(EffectSpec);
	EffectSpecHandle = FAFEffectSpecHandle::Generate(EffectSpec);

	InOutSpec = EffectSpecHandle;
}
void UGABlueprintLibrary::ApplyEffectFromSpec(UPARAM(Ref) FAFPropertytHandle& InEffect, UPARAM(Ref) FAFEffectSpecHandle& InSpec)
{

}
void UGABlueprintLibrary::ModifyAttributeSimple(
	UPARAM(Ref) FAFEffectSpecHandle& InSpec
	, UObject* Target)
{
	if (!InSpec.IsValid())
	{
		return;
	}

	IAFAttributeInterface* Attr = Cast<IAFAttributeInterface>(Target);
	IAFSimpleInterface* Simpl = Cast<IAFSimpleInterface>(Target);

	if (!Attr || !Simpl)
		return;

	FGAEffectHandle Handle = FGAEffectHandle::GenerateHandle();

	FGAEffect Effect = FGAEffect(InSpec.GetPtr(), Handle);

	FGAEffectMod Mod = InSpec.GetPtr()->GetModifier();

	//Generate HAndle.
	//Add to simple container.
	//Modify Attribute
}