// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GASGlobals.generated.h"
/**
 * 
 */
USTRUCT()
struct GAMEABILITIES_API FGASAbilitySetItem
{
	GENERATED_USTRUCT_BODY()
public:
	/* Index in this array coresponds directly FGASActiveAbility::ActiveAbilities Index*/
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class UGASAbilityBase>> AbilityClass;
};

USTRUCT()
struct GAMEABILITIES_API FGASAbilitySetSlot
{
	GENERATED_USTRUCT_BODY()
public:
	/* Index in this array coresponds directly FGASActiveAbility::ActiveAbilities Index*/
	UPROPERTY(EditAnywhere)
		TArray<FGASAbilitySetItem> Abilities;
};

USTRUCT()
struct GAMEABILITIES_API FGASAbilitySetContainer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UGASInputOverride> CustomInputClass;
	/* Index in array corresponds directly to index in FGASActiveAbilitySet::Abilities */
	UPROPERTY(EditAnywhere)
		TArray<FGASAbilitySetItem> Abilities;
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASAbilityNotifyData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		float TEMP;
};