// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "../GAAbilitiesComponent.h"


#include "GABlueprintLibrary.h"
#include "../IGAAbilities.h"
#include "GAEffectExtension.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UGABlueprintLibrary::ApplyGameEffectProperty(FGAEffectProperty& Handle)
{

}
void UGABlueprintLibrary::ApplyGameEffect(FGAEffectHandle Handle)
{
	if (!Handle.IsValid())
		return;

	FGAEffectContext& Context = Handle.GetContext();

//	Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle);
}

void UGABlueprintLibrary::ApplyGameEffectToObject(UPARAM(ref) FGAEffectProperty& InEffect,
	class UObject* Target, class APawn* Instigator,
	UObject* Causer)
{


	InEffect.Handle = ApplyEffectToObject(InEffect.GetSpec(), InEffect, InEffect.Handle, Target, Instigator, Causer);
}

void UGABlueprintLibrary::ApplyGameEffectToActor(UPARAM(ref) FGAEffectProperty& InEffect,
	class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	InEffect.Handle = ApplyEffectToActor(InEffect.GetSpec(), InEffect, InEffect.Handle, Target, Instigator, Causer);
}

void UGABlueprintLibrary::ApplyGameEffectToLocation(UPARAM(ref) FGAEffectProperty& InEffect,
	const FHitResult& Target, class APawn* Instigator,
	UObject* Causer)
{
	InEffect.Handle = ApplyEffectFromHit(InEffect.GetSpec(), InEffect, InEffect.Handle, Target, Instigator, Causer);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffect(UGAGameEffectSpec* SpecIn, FGAEffectProperty& InEffect,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer, const FHitResult& HitIn)
{
	if (!SpecIn)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Effect Spec"));
		return FGAEffectHandle();
	}
	InEffect.ApplicationRequirement = InEffect.GetSpec()->ApplicationRequirement.GetDefaultObject();
	InEffect.Application = InEffect.GetSpec()->Application.GetDefaultObject();
	InEffect.Execution = InEffect.GetSpec()->ExecutionType.GetDefaultObject();
	FGAEffectContext Context = MakeContext(Target, Instigator, Causer, HitIn);
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
	UGAAbilitiesComponent* Target2 = Context.TargetComp.Get();
	if (!Target2->HasAll(SpecIn->RequiredTags))
	{
		return FGAEffectHandle();
	}
	if (SpecIn->DenyTags.Num() > 0 && Target2->HasAll(SpecIn->DenyTags))
	{
		return FGAEffectHandle();
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
	InEffect.Duration = SpecIn->Duration.GetFloatValue(Context);
	InEffect.Period = SpecIn->Period.GetFloatValue(Context);
	Context.InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffectRef(), HandleIn, InEffect);
	return HandleIn;
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectFromHit(UGAGameEffectSpec* SpecIn, FGAEffectProperty& InEffect,
	FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffect(SpecIn, InEffect, HandleIn, Target.GetActor(), Instigator, Causer, Target);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToActor(UGAGameEffectSpec* SpecIn, FGAEffectProperty& InEffect,
	FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(SpecIn, InEffect, HandleIn, Target, Instigator, Causer, Hit);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToObject(UGAGameEffectSpec* SpecIn, FGAEffectProperty& InEffect,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(SpecIn, InEffect, HandleIn, Target, Instigator, Causer, Hit);
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

	FGAEffectContext Context = MakeContext(Target, Instigator, Causer, HitIn);
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
FGAEffectHandle UGABlueprintLibrary::ApplyEffect(const FGAEffectHandle& HandleIn)
{
	//HandleIn.GetContextRef().InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffect(), HandleIn);
	return HandleIn;
}
FGAEffectContext UGABlueprintLibrary::MakeContext(class UObject* Target, class APawn* Instigator, UObject* Causer, const FHitResult& HitIn)
{
	IIGAAbilities* targetAttr = Cast<IIGAAbilities>(Target);
	IIGAAbilities* instiAttr = Cast<IIGAAbilities>(Instigator);
	if (!targetAttr && !instiAttr)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Target and Instigator does not implement IIGAAbilities interface"));
		return FGAEffectContext();
	}
	UGAAbilitiesComponent* targetComp = nullptr;
	UGAAbilitiesComponent* instiComp = nullptr;
	if (targetAttr)
	{
		targetComp = targetAttr->GetAbilityComp();
	}
	if (instiAttr)
	{
		instiComp = instiAttr->GetAbilityComp();
	}

	FVector location = targetComp->GetOwner()->GetActorLocation();
	FGAEffectContext Context(targetAttr->GetAttributes(), instiAttr->GetAttributes(),
		location, Target, Causer,
		Instigator, targetComp, instiComp);
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

UGAAbilitiesComponent* UGABlueprintLibrary::GetTargetComponent(const FGAEffectHandle& InHandle)
{
	return InHandle.GetContextRef().InstigatorComp.Get();
}

UGAAbilitiesComponent* UGABlueprintLibrary::GetInstigatorComponent(const FGAEffectHandle& InHandle)
{
	return InHandle.GetContextRef().TargetComp.Get();
}
