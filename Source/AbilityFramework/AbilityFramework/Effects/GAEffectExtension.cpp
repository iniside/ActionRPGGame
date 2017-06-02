// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "../GAAbilitiesComponent.h"
#include "GAEffectExtension.h"

UGAEffectExtension::UGAEffectExtension(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}
void UGAEffectExtension::SetParameters(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}
void UGAEffectExtension::BeginEffect()
{
	UWorld* World = GetWorld();
	//if there is no valid world, don't do anything.
	if (World)
	{
		NativeOnEffectApplied();
		if (Duration > 0)
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &UGAEffectExtension::InternallSelfRemoveEffect);

			TimerManager.SetTimer(DurationTimerHandle, Del, Duration, false);
		}
		if (Period > 0)
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &UGAEffectExtension::NativeOnEffectPeriod);

			TimerManager.SetTimer(PeriodTimerHandle, Del, Period, true);
		}
	}
}

FGAEffectHandle UGAEffectExtension::ApplyEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn)
{
	FGAEffectHandle handle = Context.InstigatorComp->MakeGameEffect(SpecIn, Context);

	//Context.InstigatorComp->ApplyEffectToTarget(handle.GetEffect(), handle);
	return handle;
}
void UGAEffectExtension::NativeOnEffectApplied()
{
	OnEffectInstanceApplied();
}
void UGAEffectExtension::NativeOnEffectPeriod()
{
	OnEffectInstancePeriod();
}
void UGAEffectExtension::NativeOnEffectExpired()
{
	InternalEffectEnded();
	OnEffectInstanceExpired();
}
void UGAEffectExtension::NativeOnEffectRemoved()
{
	InternalEffectEnded();
	OnEffectInstanceRemoved();
}
void UGAEffectExtension::InternallSelfRemoveEffect()
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
void UGAEffectExtension::InternalEffectEnded()
{
	UGAAbilitiesComponent* OwningComp = Context.TargetComp.Get();
	if (!OwningComp)
		return;

	//for (FGAEffectHandle& Handle : OwnedEffects)
	//{
	//	OwningComp->RemoveEffect(Handle);
	//}

	MarkPendingKill();
}

UWorld* UGAEffectExtension::GetWorld() const
{
	if (Context.Target.IsValid())
		return Context.Target->GetWorld();

	return nullptr;
}