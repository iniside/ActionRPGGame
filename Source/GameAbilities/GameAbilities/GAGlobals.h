// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/NetSerialization.h"
#include "GameplayTags.h"
#include "GAGlobals.generated.h"
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
		TSubclassOf<class UGAAbilityBase> AbilityClass;

	UPROPERTY(EditAnywhere)
		FGameplayTag Binding;
};

USTRUCT()
struct GAMEABILITIES_API FGASAbilitySetContainer
{
	GENERATED_USTRUCT_BODY()
public:
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

USTRUCT()
struct GAMEABILITIES_API FGASAbilityHit : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FHitResult Hit;

	FGASAbilityHit()
	{}

	FGASAbilityHit(const FHitResult& HitIn)
		: Hit(HitIn)
	{};
};

USTRUCT()
struct GAMEABILITIES_API FGASAbilityHitArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGASAbilityHit> Hits;

	inline void AddHit(const FHitResult& HitIn)
	{
		Hits.Add(FGASAbilityHit(HitIn));
	}

	inline void Empty()
	{
		Hits.Empty();
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGASAbilityHit, FGASAbilityHitArray>(Hits, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FGASAbilityHitArray > : public TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};