// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameplayTaskOwnerInterface.h"
#include "GameplayTaskTypes.h"
#include "GameplayTask.h"
#include "GameplayTasksComponent.h"
#include "GASGlobals.h"
#include "GASAbilitiesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGASOnActiveAbilityAdded, int32, int32);
DECLARE_DELEGATE_OneParam(FGASMontageGenericDelegate, const FGASAbilityNotifyData&);
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
struct GAMEABILITIES_API FGASMontageRepData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		UAnimMontage* CurrentMontage;

	UPROPERTY()
		FName SectionName;

	UPROPERTY()
		uint8 ForceRep;
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

	/* by default there is single active ability, at index 0 */
	UPROPERTY()
		TArray<class UGASAbilityBase*> ActiveAbilities;

	void SetAbility(class UGASAbilityBase* Ability, int32 SetIndexIn, int32 SlotIndexIn, int32 SubSlotIndex = 0);
	
	void Reset(TSubclassOf<class UGASAbilityBase> AbilityClass);

	void OnAbilityAdded()
	{
	};

	FGASActiveAbility()
		: SetIndex(INDEX_NONE),
		SlotIndex(INDEX_NONE)
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

	void SetAbility(class UGASAbilityBase* Ability, int32 SetIndexIn, int32 SlotIndexIn, int32 SubSlotIndex = 0)
	{
		Abilities[SlotIndexIn].SetAbility(Ability, SetIndexIn, SlotIndexIn, SubSlotIndex);
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
				if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities.Num() > 0
					&& AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[0])
				{
					return true;
				}
			}
		}
		return false;
	}

	void AddAbility(class UGASAbilityBase* Ability, int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0)
	{
		if (AbilitySets.IsValidIndex(SetIndex))
		{
			if (AbilitySets[SetIndex].Abilities.IsValidIndex(SlotIndex))
			{
				AbilitySets[SetIndex].SetAbility(Ability, SetIndex, SlotIndex, SubSlotIndex);
			}
		}
	}

	bool CanUseAbility(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
	bool IsWaitingForConfirm(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
	void ConfirmAbility(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
	void OnNativeInputPressed(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
	void OnNativeInputReleased(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
	void RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex);
	void ClearAndResizeAbilitiesCount(int32 SetIndex, int32 SlotIndex, int32 NewSize);
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

	FGASMontageGenericDelegate OnAbilityStartNotify;
	FGASMontageGenericDelegate OnAbilityEndNotify;
	FGASMontageGenericDelegate OnAbilityNotify;
	FGASMontageGenericDelegate OnAbilityNotifyStateStart;
	FGASMontageGenericDelegate OnAbilityNotifyStateTick;
	FGASMontageGenericDelegate OnAbilityNotifyStateEnd;
private:
	class IIGIPawn* PawnInterface;

	UPROPERTY(ReplicatedUsing=OnRep_PlayMontage)
		FGASMontageRepData RepMontage;
	UFUNCTION()
		void OnRep_PlayMontage();
public:
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
	void PlayMontage(UAnimMontage* MontageIn, FName SectionName);
public:
	/*
		Checks on client and server, if we can activate ability. Called from InputPressed
	*/
	virtual bool CanActivateAbility();

	virtual void InitializeComponent() override;

	inline class UGASAbilityBase* GetGASAbility(int32 IndexIn)
	{
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

	void NativeAddAbilitiesFromSet(TSubclassOf<class UGASAbilitySet> AbilitySet, int32 SetIndex);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
	void BP_AddAbilitiesFromSet(TSubclassOf<class UGASAbilitySet> AbilitySet, int32 SetIndex);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);



	void GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass);

	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
protected:
	void InitializeInstancedAbilities();
	void InstanceAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex = 0);
};



