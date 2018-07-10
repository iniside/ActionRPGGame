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
#include "AFEffectsComponent.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToObject(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, class UObject* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<UObject*> Targets;
	Targets.Add(Target);
	return ApplyEffectToObject(InEffect, Targets, Instigator, Causer, Modifier);
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToActor(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, class AActor* Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<AActor*> Targets;
	Targets.Add(Target);
	return ApplyEffectToActor(InEffect, Targets, Instigator, Causer, Modifier);
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToLocation(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, const FHitResult& Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<FHitResult> Targets;
	Targets.Add(Target);
	return ApplyEffectFromHit(InEffect, Targets, Instigator, Causer, Modifier);
}


TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToObjects(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, TArray<class UObject*> Targets
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToObject(InEffect, Targets, Instigator, Causer, Modifier);
}
TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToActors(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, TArray<class AActor*> Targets
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectToActor(InEffect, Targets, Instigator, Causer, Modifier);
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyGameEffectToTargets(
	UPARAM(Ref) FAFPropertytHandle& InEffect
	, const TArray<FHitResult>& Targets
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	return ApplyEffectFromHit(InEffect, Targets, Instigator, Causer, Modifier);
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

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyEffect(
	FAFPropertytHandle& InEffect
	, TArray<class UObject*> Targets
	, class APawn* Instigator
	, UObject* Causer
	, const TArray<FHitResult>& HitsIn
	, const FAFFunctionModifier& Modifier)
{
	bool bCheckHits = Targets.Num() == HitsIn.Num();
	TArray<FGAEffectHandle> Handles;
	for (int32 Idx = 0; Idx < Targets.Num(); Idx++)
	{
		UObject* Target = Targets[Idx];
		FHitResult HitIn;
		if (bCheckHits)
		{
			HitIn = HitsIn[Idx];
		}
		

		ENetMode NetMode = GEngine->GetNetMode(Instigator->GetWorld());

		UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect START, NetMode %s"), *NetModeToString(NetMode));
		UE_LOG(GameAttributesEffects, Error, TEXT("-----"));
		IAFAbilityInterface* Ability = Cast<IAFAbilityInterface>(Causer);
		if (!Ability)
		{
			UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect Effects must be applied trough Ability"));
			return Handles;
		}

		IAFAbilityInterface* TargetInterface = Cast<IAFAbilityInterface>(Target);


		if (!InEffect.GetClass().GetDefaultObject()->Application.GetDefaultObject()->CanApply(TargetInterface, InEffect.GetClass()))
		{
			return Handles;
		}
		bool bReusedParams = false;
		bool bPeriodicEffect = false;
		
		FAFEffectSpecHandle EffectSpecHandle;
		FAFEffectParams Params(InEffect);
		InEffect.InitializeIfNotInitialized(Instigator, Causer);
		Params.Context = InEffect.GetPtr()->GetContextCopy(Target, HitIn);

		UAFEffectsComponent* Target2 = Params.Context.GetTargetEffectsComponent();

		UGAGameEffectSpec* Spec = InEffect.GetSpecData();

		if (!Target2->HaveEffectRquiredTags(Spec->ApplicationRequiredTags))
		{
			return Handles;
		}
		if (Target2->DenyEffectApplication(Spec->DenyTags))
		{
			return Handles;
		}

		if ((InEffect.GetDuration() > 0 || InEffect.GetPeriod() > 0))
		{
			bPeriodicEffect = true;
		}

		if (!InEffect.IsInitialized())
		{
			UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
			return Handles;
		}



		UE_LOG(GameAttributesEffects, Log, TEXT("MakeOutgoingSpecObj: Created new Context: %s"), *Params.Context.ToString());

		FGAEffect effect;
		effect.World = Instigator->GetWorld();
		if (Ability)
		{
			effect.PredictionHandle = Ability->GetPredictionHandle();
		}
		IAFAbilityInterface* InstigatorInterface = Cast<IAFAbilityInterface>(Instigator);

		
		/*if (Params.EffectSpec.GetPtr())
		{
			if (Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target.IsValid())
			{
				UE_LOG(GameAttributesEffects, Log, TEXT("  Pre Spec Target: %s"), *Params.EffectSpec.GetPtr()->GetContext().GetPtr()->Target->GetName());
			}
		}*/

		Params.EffectSpec = InEffect.GetPtr()->GetSpecCopy();
		AddTagsToEffect(&Params.EffectSpec);

		Params.bRecreated = bReusedParams;
		Params.bPeriodicEffect = bPeriodicEffect;

		FGAEffectHandle NewHandle = InstigatorInterface->ApplyEffectToTarget(effect, Params, Modifier);
		Handles.Add(NewHandle);
		UE_LOG(GameAttributesEffects, Error, TEXT("-----"));
		UE_LOG(GameAttributesEffects, Error, TEXT("UGABlueprintLibrary::ApplyEffect END"));

	}
	return Handles;
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyEffectFromHit(
	FAFPropertytHandle& InEffect
	, const TArray<FHitResult>& Targets
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<UObject*> ActorTargets;
	for (const FHitResult& Hit : Targets)
	{
		ActorTargets.Add(Hit.GetActor());
	}
	return ApplyEffect(InEffect, ActorTargets, Instigator, Causer, Targets, Modifier);
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyEffectToActor(
	FAFPropertytHandle& InEffect
	, TArray<class AActor*> Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<FHitResult> Hits;
	TArray<UObject*> ObjTargets;
	for (AActor* A : Target)
	{
		ObjTargets.Add(A);
	}
	return ApplyEffect(InEffect, ObjTargets, Instigator, Causer, Hits, Modifier);
}

TArray<FGAEffectHandle> UGABlueprintLibrary::ApplyEffectToObject(
	FAFPropertytHandle& InEffect
	, TArray<class UObject*> Target
	, class APawn* Instigator
	, UObject* Causer
	, const FAFFunctionModifier& Modifier)
{
	TArray<FHitResult> Hits;
	return ApplyEffect(InEffect, Target, Instigator, Causer, Hits, Modifier);
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