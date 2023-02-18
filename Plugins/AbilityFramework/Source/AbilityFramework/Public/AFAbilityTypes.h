// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/NetSerialization.h"
#include "GameplayTags.h"
#include "AFAbilityTypes.generated.h"

class UGAAbilityBase;

USTRUCT(BlueprintType)
struct FAFAbilitySpecHandle
{
	GENERATED_BODY()
		friend struct FAFAbilitySpec;
public:
	UPROPERTY()
		uint32 Handle;

	static FAFAbilitySpecHandle GenerateHandle()
	{
		static uint32 HandleID = 0;
		HandleID++;
		FAFAbilitySpecHandle Handle;
		Handle.Handle = HandleID;
		return Handle;
	}
public:
	bool IsValid() const
	{
		return Handle != 0;
	}
	bool operator==(const FAFAbilitySpecHandle& ROther) const
	{
		return Handle == ROther.Handle;
	}
	friend uint32 GetTypeHash(const FAFAbilitySpecHandle& InHandle)
	{
		return InHandle.Handle;
	}
};


/*
	Handle is created on calling client and then send to server. If/when server instance ability and send back it to client
	it will use exactly the same handle.
*/
USTRUCT()
struct ABILITYFRAMEWORK_API FAFAbilitySpec : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	friend struct FAFAbilitySpecHandle;
	UPROPERTY()
		UGAAbilityBase* Ability;
	UPROPERTY()
		uint8 InputID;
	/* Server generated handle that is valid on both client and server. Use it to find ability */
	UPROPERTY()
		FAFAbilitySpecHandle Handle;
	/* Client generated handle that is only used temporarily to fire events on owning client. */
	UPROPERTY()
		FAFAbilitySpecHandle ClientHandle;

	void PreReplicatedRemove(const struct FAFAbilityContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FAFAbilityContainer& InArraySerializer);
	void PostReplicatedChange(const struct FAFAbilityContainer& InArraySerializer);

	bool operator==(const FAFAbilitySpec& ROther) const
	{
		return Handle == ROther.Handle;
	}
	bool operator==(const FAFAbilitySpecHandle& ROther) const
	{
		return Handle == ROther;
	}
	friend uint32 GetTypeHash(const FAFAbilitySpec& InSpec)
	{
		return InSpec.Handle.Handle;
	}
};

USTRUCT()
struct ABILITYFRAMEWORK_API FAFAbilityContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:
		TWeakObjectPtr<class UAFAbilityComponent> AbilitiesComp;
	UPROPERTY()
		TArray<FAFAbilitySpec> ActivatableAbilities;

	TMap<FAFAbilitySpecHandle, FAFAbilitySpec> SpecMap;

	TMap<uint8, FAFAbilitySpec> InputToAbilitySpec;

	UGAAbilityBase* AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn, const FAFAbilitySpecHandle Handle, const FAFAbilitySpecHandle ClientHandle);


	void RemoveAbility(const FAFAbilitySpecHandle AbilityIn);
	
	UGAAbilityBase* GetAbility(FAFAbilitySpecHandle InAbiltyPtr);
	
	void HandleInputPressed(const uint8 InputID, const FAFPredictionHandle& InPredictionHandle);
	void HandleInputReleased(const uint8 InputID);

	bool AbilityExists(FAFAbilitySpecHandle InAbiltyPtr) const
	{
		return SpecMap.Contains(InAbiltyPtr);
	}
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FAFAbilitySpec, FAFAbilityContainer>(ActivatableAbilities, DeltaParms, *this);
	}
	void BindAbilityToInputIDs(const FAFAbilitySpecHandle Handle, TArray<uint8> InputIDs);
};

template<>
struct TStructOpsTypeTraits< FAFAbilityContainer > : public TStructOpsTypeTraitsBase2<FAFAbilityContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};