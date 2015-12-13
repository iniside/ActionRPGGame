// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameplayTaskOwnerInterface.h"
#include "GameplayTaskTypes.h"
#include "GameplayTask.h"
#include "GameplayTasksComponent.h"
#include "GASAbilitiesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGASOnActiveAbilityAdded, int32, int32);
/* TODO:: Implement fast serialization for structs. */
/**/
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

	/* by default there is single active ability, at index 0 */
	UPROPERTY()
		TArray<class UGASAbilityBase*> ActiveAbilities;

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
	/* 
		Class designited for handling input for abilities in set.
		Makes possible to encapsule behaviors like chained abilities and combos (press 1,1 and then 3
		to get special ability), within single object, which will depend on abilities in particular set.
	*/
	UPROPERTY()
		class UGASInputOverride* InputOverride;

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
	//SetIndex, SlotIndex, AbilityIndex (??? LOLLMAO ROFL?)
	UPROPERTY()
		TArray<FGASActiveAbilitySet> AbilitySets2;

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
	void OnNativeInputReleased(int32 SetIndex, int32 SlotIndex);
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

	/* Maximum amount of slots in single set.*/
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaxSlots;
	/* 
		Maximum amount of abilities in single slot. If left at 0 or less it will assume
		one ability per slot.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaxAbilitiesInSlot;
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
		FGASActiveAbilitiesConfig AbilitiesConfig;
	/* Ability which is currently being executed. */
	UPROPERTY(BlueprintReadOnly, Category = "Game Abilities")
		class UGASAbilityBase* ExecutingAbility;

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

	void NativeInputPressed(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNativeInputPressed(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	virtual void ServerNativeInputPressed_Implementation(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	virtual bool ServerNativeInputPressed_Validate(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Input Released"), Category = "Game Abilities")
		void BP_InputReleased(int32 SetIndex, int32 SlotIndex);

	void NativeInputReleased(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNativeInputReleased(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	virtual void ServerNativeInputReleased_Implementation(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);
	virtual bool ServerNativeInputReleased_Validate(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_AddAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_AddAbility2(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);
	/*
		Should be called on server.
		Adds new ability to ActiveAbilities;
	*/
	int32 AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass);
	void AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 AbilityIndex = 0);

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



