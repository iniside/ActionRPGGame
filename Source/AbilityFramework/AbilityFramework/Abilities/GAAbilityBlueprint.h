// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Blueprint.h"
#include "GAAbilityBlueprint.generated.h"

/**
 * Game Ability Blueprint
 */

UCLASS(BlueprintType)
class ABILITYFRAMEWORK_API UGAAbilityBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR

	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override
	{
		return false;
	}
	// End of UBlueprint interface

	/** Returns the most base gameplay ability blueprint for a given blueprint (if it is inherited from another ability blueprint, returning null if only native / non-ability BP classes are it's parent) */
	static UGAAbilityBlueprint* FindRootGameplayAbilityBlueprint(UGAAbilityBlueprint* DerivedBlueprint);

#endif
};
