// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"

#include "Abilities/GAAbilityBase.h"
#include "Effects/GAEffectField.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "AFBlueprintFunctionLibrary.h"

void UAFBlueprintFunctionLibrary::TriggerAbilityPressedByTag(UObject* Target,
	const FGameplayTag& AbilityTag, FGameplayTag ActionTag)
{
	IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(Target);
	if (!Interface)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityPressedByTag: Invalid Target"));
		return;
	}
	UAFAbilityComponent* Comp = Interface->GetAbilityComp();
	if (!Comp)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityPressedByTag: Target %s InvalidComponent"), *Target->GetName());
		return;
	}

	//Comp->NativeInputPressed(ActionTag);
}
void UAFBlueprintFunctionLibrary::TriggerAbilityReleasedByTag(UObject* Target,
	const FGameplayTag& AbilityTag, FGameplayTag ActionTag)
{
	IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(Target);
	if (!Interface)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityReleasedByTag: Invalid Target"));
		return;
	}
	UAFAbilityComponent* Comp = Interface->GetAbilityComp();
	if (!Comp)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TriggerAbilityReleasedByTag: Target %s InvalidComponent"), *Target->GetName());
		return;
	}

	//Comp->NativeInputReleased(ActionTag);
}