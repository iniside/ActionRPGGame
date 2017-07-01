// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"

#include "GAAbilityBase.h"
#include "../Effects/GAEffectField.h"
#include "../IGAAbilities.h"
#include "../GAAbilitiesComponent.h"
#include "AFBlueprintFunctionLibrary.h"

void UAFBlueprintFunctionLibrary::TriggerAbilityPressedByTag(UObject* Target,
	const FGameplayTag& AbilityTag, FGameplayTag ActionTag)
{
	IIGAAbilities* Interface = Cast<IIGAAbilities>(Target);
	if (!Interface)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityPressedByTag: Invalid Target"));
		return;
	}
	UGAAbilitiesComponent* Comp = Interface->GetAbilityComp();
	if (!Comp)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityPressedByTag: Target %s InvalidComponent"), *Target->GetName());
		return;
	}

	Comp->NativeInputPressed(AbilityTag, ActionTag);
}
void UAFBlueprintFunctionLibrary::TriggerAbilityReleasedByTag(UObject* Target,
	const FGameplayTag& AbilityTag, FGameplayTag ActionTag)
{
	IIGAAbilities* Interface = Cast<IIGAAbilities>(Target);
	if (!Interface)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityReleasedByTag: Invalid Target"));
		return;
	}
	UGAAbilitiesComponent* Comp = Interface->GetAbilityComp();
	if (!Comp)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityReleasedByTag: Target %s InvalidComponent"), *Target->GetName());
		return;
	}

	Comp->NativeInputReleased(AbilityTag, ActionTag);
}