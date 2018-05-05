// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/NetSerialization.h"
#include "GameplayTags.h"
#include "AFAbilityTypes.generated.h"

class UGAAbilityBase;

USTRUCT()
struct ABILITYFRAMEWORK_API FAFAbilityItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
		UGAAbilityBase* Ability;
	UPROPERTY()
		TSoftClassPtr<UGAAbilityBase> AbilityClass;

	FAFAbilityItem()
	{};

	FAFAbilityItem(UGAAbilityBase* InAbility, TSoftClassPtr<UGAAbilityBase> InAbilityClass)
		: Ability(InAbility)
		, AbilityClass(InAbilityClass)
	{}

	void PreReplicatedRemove(const struct FAFAbilityContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FAFAbilityContainer& InArraySerializer);
	void PostReplicatedChange(const struct FAFAbilityContainer& InArraySerializer);

	const bool operator==(const TSoftClassPtr<UGAAbilityBase>& OtherAbility) const
	{
		return AbilityClass == OtherAbility;
	}

	const bool operator==(UGAAbilityBase* OtherAbility) const
	{
		return Ability == OtherAbility;
	}
	const bool operator==(const FAFAbilityItem& OtherItem) const
	{
		return Ability == OtherItem.Ability;
	}
};

USTRUCT()
struct ABILITYFRAMEWORK_API FAFAbilityContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TArray<FAFAbilityItem> AbilitiesItems;

	TWeakObjectPtr<class UAFAbilityComponent> AbilitiesComp;
	TMap<FGameplayTag, bool> BlockedInput;

	TMap<TSoftClassPtr<UGAAbilityBase>, FGameplayTag> AbilityToInput;

	//Custom binding, for server side validation.

	//ActionInput, AbilityClassPtr
	TMap<FGameplayTag, TSoftClassPtr<UGAAbilityBase>> ActionToAbility;

	//maybe.. replace SoftClassPtr with FObjectKey ?

	//AbilityTag, ActionInput 
	TMap<TSoftClassPtr<UGAAbilityBase>, TArray<FGameplayTag>> AbilityToAction;

	//abilityTag, Ability Ptr
	TMap<TSoftClassPtr<UGAAbilityBase>, UGAAbilityBase*> AbilitiesInputs;

	TMap<TSoftClassPtr<UGAAbilityBase>, UGAAbilityBase*> TagToAbility;

	void SetBlockedInput(const FGameplayTag& InActionName, bool bBlock);
	UGAAbilityBase* AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn
		, TSoftClassPtr<class UGAAbilityBase> InClassPtr);

	void AddAbilityFromObject(class UGAAbilityBase* AbilityIn, TSoftClassPtr<class UGAAbilityBase> InClassPtr);

	void RemoveAbility(const TSoftClassPtr<UGAAbilityBase>& AbilityIn);

	void SetAbilityToAction(const TSoftClassPtr<UGAAbilityBase>& InAbiltyPtr, const TArray<FGameplayTag>& InInputTag);
	void RemoveAbilityFromAction(const TSoftClassPtr<UGAAbilityBase>& InAbiltyPtr);
	TSoftClassPtr<UGAAbilityBase> IsAbilityBoundToAction(const FGameplayTag& InInputTag);

	UGAAbilityBase* GetAbility(TSoftClassPtr<UGAAbilityBase> InAbiltyPtr);

	void HandleInputPressed(FGameplayTag ActionName, const FAFPredictionHandle& InPredictionHandle);
	void HandleInputReleased(FGameplayTag ActionName);

	void TriggerAbylityByTag(TSoftClassPtr<UGAAbilityBase> InTag);

	bool AbilityExists(TSoftClassPtr<UGAAbilityBase> InAbiltyPtr) const
	{
		return AbilityToInput.Contains(InAbiltyPtr);
	}
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FAFAbilityItem, FAFAbilityContainer>(AbilitiesItems, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FAFAbilityContainer > : public TStructOpsTypeTraitsBase2<FAFAbilityContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};