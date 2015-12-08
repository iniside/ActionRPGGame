// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameplayTaskOwnerInterface.h"
#include "GameplayTaskTypes.h"
#include "GameplayTask.h"
#include "GameplayTasksComponent.h"
#include "GASAbilitiesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGASOnActiveAbilityAdded, int32, int32);

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASOwnedAbilities
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TSubclassOf<class UGASAbilityBase> AbilityClass;
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbility
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int32 SetIndex;
	UPROPERTY()
		int32 SlotIndex;

	UPROPERTY()
	class UGASAbilityBase* ActiveAbility;

	void SetAbility(class UGASAbilityBase* Ability, int32 SetIndexIn, int32 SlotIndexIn)
	{
		ActiveAbility = Ability;
		SetIndex = SetIndexIn;
		SlotIndex = SlotIndexIn;
	}
	
	void Reset();

	void OnAbilityAdded()
	{
	};

	FGASActiveAbility()
		: SetIndex(INDEX_NONE),
		SlotIndex(INDEX_NONE),
		ActiveAbility(nullptr)
	{
	};
};
/*
	Contains sets of active abilities, which might coresspond to particular item slots/ability bars/etc.
*/
USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbilitySet
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGASActiveAbility> Abilities;

	void SetAbility(class UGASAbilityBase* Ability, int32 SetIndexIn, int32 SlotIndexIn)
	{
		Abilities[SlotIndexIn].SetAbility(Ability, SetIndexIn, SlotIndexIn);
	}
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbilityContainer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGASActiveAbilitySet> AbilitySets;
	bool IsValid(int32 SetIndex, int32 SlotIndex)
	{
		if (AbilitySets.IsValidIndex(SetIndex))
		{
			if (AbilitySets[SetIndex].Abilities.IsValidIndex(SlotIndex))
			{
				if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbility)
				{
					return true;
				}
			}
		}
		return false;
	}

	void AddAbility(class UGASAbilityBase* Ability, int32 SetIndex, int32 SlotIndex)
	{
		if (AbilitySets.IsValidIndex(SetIndex))
		{
			if (AbilitySets[SetIndex].Abilities.IsValidIndex(SlotIndex))
			{
				AbilitySets[SetIndex].SetAbility(Ability, SetIndex, SlotIndex);
			}
		}
	}

	bool CanUseAbility(int32 SetIndex, int32 SlotIndex);
	bool IsWaitingForConfirm(int32 SetIndex, int32 SlotIndex);
	void ConfirmAbility(int32 SetIndex, int32 SlotIndex);
	void OnNativeInputPressed(int32 SetIndex, int32 SlotIndex);
	void RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex);
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASAbilitySetConfig
{
	GENERATED_USTRUCT_BODY()
public:
	/* Name of the set. It's only for description purpose. */
	UPROPERTY(EditAnywhere, Category = "Config")
		FString SetName;

	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaxAbilities;
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbilitiesConfig
{
	GENERATED_USTRUCT_BODY()
public:
	/* Each element in this array coresponds to single set of abilities in FGASActiveAbilityContainer */
	UPROPERTY(EditAnywhere, Category = "Config")
		TArray<FGASAbilitySetConfig> Sets;

	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaxAbilities;
};

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEABILITIES_API UGASAbilitiesComponent : public UGameplayTasksComponent// UActorComponent
{
	GENERATED_BODY()
public:
	UGASAbilitiesComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(ReplicatedUsing = OnRep_InstancedAbilities)
		FGASActiveAbilityContainer ActiveAbilityContainer;

	UPROPERTY(ReplicatedUsing = OnRep_InstancedAbilities)
		TArray<FGASActiveAbility> InstancedAbilities;
	UFUNCTION()
		void OnRep_InstancedAbilities();

	UPROPERTY(EditAnywhere, Category = "Config")
		FGASActiveAbilitiesConfig InstancedAbilitiesConfig;

	/*
		True if player is currently casting/channeling/activating(?)
		any ability.
	*/
	bool bIsAnyAbilityActive;
private:
	class IIGIPawn* PawnInterface;
public:
	/*
		Checks on client and server, if we can activate ability. Called from InputPressed
	*/
	virtual bool CanActivateAbility();

	virtual void InitializeComponent() override;

	inline class UGASAbilityBase* GetGASAbility(int32 IndexIn)
	{
		if (InstancedAbilities.IsValidIndex(IndexIn))
			return InstancedAbilities[IndexIn].ActiveAbility;
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Input Pressed"), Category = "Game Abilities")
		void BP_InputPressed(int32 SetIndex, int32 SlotIndex);

	void NativeInputPressed(int32 SetIndex, int32 SlotIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNativeInputPressed(int32 SetIndex, int32 SlotIndex);
	virtual void ServerNativeInputPressed_Implementation(int32 SetIndex, int32 SlotIndex);
	virtual bool ServerNativeInputPressed_Validate(int32 SetIndex, int32 SlotIndex);

	void InputReleased(int32 AbilityId);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputReleased(int32 AbilityId);
	virtual void ServerInputReleased_Implementation(int32 AbilityId);
	virtual bool ServerInputReleased_Validate(int32 AbilityId);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_AddAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_AddAbility2(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);
	/*
		Should be called on server.
		Adds new ability to ActiveAbilities;
	*/
	int32 AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass);
	void AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex);

	void RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex);
	void RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);



	void GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);

	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
protected:
	void InitializeInstancedAbilities();
};



