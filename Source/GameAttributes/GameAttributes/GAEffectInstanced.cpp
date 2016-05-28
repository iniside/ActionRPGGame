// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributeComponent.h"
#include "GAEffectInstanced.h"

UGAEffectInstanced::UGAEffectInstanced(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}
void UGAEffectInstanced::SetParameters(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}
void UGAEffectInstanced::BeginEffect()
{
	UWorld* World = GetWorld();
	//if there is no valid world, don't do anything.
	if (World)
	{
		NativeOnEffectApplied();
		if (Duration > 0)
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &UGAEffectInstanced::InternallSelfRemoveEffect);

			TimerManager.SetTimer(DurationTimerHandle, Del, Duration, false);
		}
		if (Period > 0)
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &UGAEffectInstanced::NativeOnEffectPeriod);

			TimerManager.SetTimer(PeriodTimerHandle, Del, Period, true);
		}
	}
}

FGAGameEffectHandle UGAEffectInstanced::ApplyEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn)
{
	FGAGameEffectHandle handle = Context.InstigatorComp->MakeGameEffect(SpecIn, Context);

	Context.InstigatorComp->ApplyEffectToTarget(handle.GetEffect(), handle);
	return handle;
}
void UGAEffectInstanced::NativeOnEffectApplied()
{
	OnEffectInstanceApplied();
}
void UGAEffectInstanced::NativeOnEffectPeriod()
{
	OnEffectInstancePeriod();
}
void UGAEffectInstanced::NativeOnEffectExpired()
{
	InternalEffectEnded();
	OnEffectInstanceExpired();
}
void UGAEffectInstanced::NativeOnEffectRemoved()
{
	InternalEffectEnded();
	OnEffectInstanceRemoved();
}
void UGAEffectInstanced::InternallSelfRemoveEffect()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(DurationTimerHandle);
		TimerManager.ClearTimer(PeriodTimerHandle);
	}
	if (Context.TargetComp.IsValid())
	{
		Context.TargetComp->RemoveInstancedFromSelf(this);
	}
	NativeOnEffectExpired();
}
void UGAEffectInstanced::InternalEffectEnded()
{
	UGAAttributeComponent* OwningComp = Context.TargetComp.Get();
	if (!OwningComp)
		return;

	for (FGAGameEffectHandle& Handle : OwnedEffects)
	{
		OwningComp->RemoveEffect(Handle);
	}

	MarkPendingKill();
}

UWorld* UGAEffectInstanced::GetWorld() const
{
	if (Context.Target.IsValid())
		return Context.Target->GetWorld();

	return nullptr;
}