// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GAEffectBlueprint.h"

//////////////////////////////////////////////////////////////////////////
// UGameplayAbilityBlueprint

UGAEffectBlueprint::UGAEffectBlueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR

/** Returns the most base gameplay ability blueprint for a given blueprint (if it is inherited from another ability blueprint, returning null if only native / non-ability BP classes are it's parent) */
UGAEffectBlueprint* UGAEffectBlueprint::FindRootGameplayAbilityBlueprint(UGAEffectBlueprint* DerivedBlueprint)
{
	UGAEffectBlueprint* ParentBP = NULL;

	// Determine if there is a gameplay ability blueprint in the ancestry of this class
	for (UClass* ParentClass = DerivedBlueprint->ParentClass; ParentClass != UObject::StaticClass(); ParentClass = ParentClass->GetSuperClass())
	{
		if (UGAEffectBlueprint* TestBP = Cast<UGAEffectBlueprint>(ParentClass->ClassGeneratedBy))
		{
			ParentBP = TestBP;
		}
	}

	return ParentBP;
}

#endif
