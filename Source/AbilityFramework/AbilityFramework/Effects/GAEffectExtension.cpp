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
}

void UGAEffectExtension::NativeOnEffectApplied()
{
	OnEffectApplied();
}
void UGAEffectExtension::NativeOnEffectExecuted()
{
	OnEffectExecuted();
}
void UGAEffectExtension::NativeOnEffectExpired()
{
	OnEffectExpired();
}
void UGAEffectExtension::NativeOnEffectRemoved()
{
	OnEffectRemoved();
}

UWorld* UGAEffectExtension::GetWorld() const
{
	if (Context.Target.IsValid())
		return Context.Target->GetWorld();

	return nullptr;
}