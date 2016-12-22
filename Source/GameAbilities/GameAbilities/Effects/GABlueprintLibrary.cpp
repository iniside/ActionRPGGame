// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GAAbilitiesComponent.h"


#include "GABlueprintLibrary.h"
#include "../IGAAbilities.h"
#include "GABlueprintLibrary.h"
#include "../GAEffectExtension.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGABlueprintLibrary::ApplyGameEffect(FGAEffectHandle Handle)
{
	if (!Handle.IsValid())
		return;

	FGAEffectContext& Context = Handle.GetContext();

	Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle);
}

void UGABlueprintLibrary::ApplyGameEffectInstance(TSubclassOf<class UGAEffectExtension> EffectClass,
	const FHitResult& Target, APawn* Instigator,
	UObject* Causer, EGAMakeSpecResult& ResultOu)
{
	UGAEffectExtension* EffectCDO = EffectClass.GetDefaultObject();
	//do not apply instanced effects with duration less than zero.
	if (EffectCDO && EffectCDO->Duration <= 0)
		return;

	FGAEffectContext Context =  MakeContext(Target.GetActor(), Instigator, Causer, Target);
	
	if (!Context.IsValid())
		return;

	UGAEffectExtension* Effect = NewObject<UGAEffectExtension>(Context.TargetComp.Get(), EffectClass);

	if (Effect)
	{
		Effect->SetParameters(Context);
	}
	Context.InstigatorComp->ApplyInstancedToTarget(Effect);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToObject(const FGAEffectSpec& SpecIn,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectToObject(SpecIn.Spec, HandleIn, Target, Instigator, Causer);
}
FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToObjectFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectToObject(SpecIn.GetDefaultObject(), HandleIn, Target, Instigator, Causer);
}
FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToActor(const FGAEffectSpec& SpecIn,
	FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectToActor(SpecIn.Spec, HandleIn, Target, Instigator, Causer);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToActorFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectToActor(SpecIn.GetDefaultObject(), HandleIn, Target, Instigator, Causer);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToLocation(const FGAEffectSpec& SpecIn,
	FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectFromHit(SpecIn.Spec, HandleIn, Target, Instigator, Causer);
}

FGAEffectHandle UGABlueprintLibrary::ApplyGameEffectToLocationFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffectFromHit(SpecIn.GetDefaultObject(), HandleIn, Target, Instigator, Causer);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffect(UGAGameEffectSpec* SpecIn,
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
	Context.InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffect(), HandleIn);
	return HandleIn;
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectFromHit(UGAGameEffectSpec* SpecIn,
	FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
	UObject* Causer)
{
	return ApplyEffect(SpecIn, HandleIn, Target.GetActor(), Instigator, Causer, Target);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToActor(UGAGameEffectSpec* SpecIn,
	FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(SpecIn, HandleIn, Target, Instigator, Causer, Hit);
}

FGAEffectHandle UGABlueprintLibrary::ApplyEffectToObject(UGAGameEffectSpec* SpecIn,
	FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
	UObject* Causer)
{
	FHitResult Hit(ForceInit);
	return ApplyEffect(SpecIn, HandleIn, Target, Instigator, Causer, Hit);
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
	HandleIn.GetContextRef().InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffect(), HandleIn);
	return HandleIn;
}
FGAEffectContext UGABlueprintLibrary::MakeContext(class UObject* Target, class APawn* Instigator, UObject* Causer, const FHitResult& HitIn)
{
	IIGAAbilities* targetAttr = Cast<IIGAAbilities>(Target);
	IIGAAbilities* instiAttr = Cast<IIGAAbilities>(Instigator);
	if (!targetAttr && !instiAttr)
	{
		UE_LOG(GameAttributesEffects, Error, TEXT("Invalid Target or Instigator"));
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