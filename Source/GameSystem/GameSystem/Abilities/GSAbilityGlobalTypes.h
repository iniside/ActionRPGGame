#pragma once
#include "GSAbilityGlobalTypes.generated.h"
/*
Struct representing available abilities to this component.
It does not represent abilities which are Instanced (ActiveAbilities array).
*/
USTRUCT()
struct GAMESYSTEM_API FGSAbilityBookSlot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TSubclassOf<class UGSAbility> AbilityClass;

	UPROPERTY()
		int32 SlotIndex;
	UPROPERTY()
		int32 TabIndex;
	/*
	If this ability is active, store index of array, in which instanced ability is contained.
	*/
	UPROPERTY()
		int32 ActiveAbilityIndex;
	/*
		Should ability be visible in UI ?
	*/
	UPROPERTY()
		bool bIsAbilityVisible;
	/*
		Is Ability active (If not, it can't be dragged for example).
	*/
	UPROPERTY()
		bool bIsAbilityActive;

	FGSAbilityBookSlot()
	{};
	FGSAbilityBookSlot(int32 SlotIndexIn, int32 TabIndexIn)
		: SlotIndex(SlotIndexIn),
		TabIndex(TabIndexIn),
		ActiveAbilityIndex(-1),
		bIsAbilityActive(false)
	{};

	FGSAbilityBookSlot(TSubclassOf<class UGSAbility> AbilityClassIn, int32 SlotIndexIn)
		: AbilityClass(AbilityClassIn),
		SlotIndex(SlotIndexIn),
		ActiveAbilityIndex(-1),
		bIsAbilityActive(false)
	{};
};
/*
It's convienice, so you can group abilities by something, Like skill type (Conjuration,
Mele, etc).
Or you can have only one Tab for everything, and implement some custom sorting.
Or you can create new tab as way for static paging.
Either way, it is not set in stone.
*/
USTRUCT()
struct FGSAbilityBookTab
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGSAbilityBookSlot> Abilities;
	UPROPERTY()
		FName TabName;
	UPROPERTY()
		int32 TabIndex;
};

USTRUCT()
struct FGSAbilityBook
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGSAbilityBookTab> AbilitiesTab;

	UPROPERTY()
		int8 ForceRep;
};


/*
For all of this shit, implement fast array serialization.
*/

USTRUCT()
struct GAMESYSTEM_API FGSAbilitySlot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int32 AbilityIndex;

	UPROPERTY()
		int32 SlotIndex;

	UPROPERTY()
		int32 SetIndex;

	FGSAbilitySlot()
		: AbilityIndex(INDEX_NONE),
		SlotIndex(INDEX_NONE),
		SetIndex(INDEX_NONE)
	{
	}
};
USTRUCT()
struct GAMESYSTEM_API FGSAbilitiesSets
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int32 SetIndex;

	UPROPERTY()
		TArray<FGSAbilitySlot> AbilitySlots;

	UPROPERTY()
		int8 RepMe;

	FGSAbilitiesSets()
	{
		SetIndex = INDEX_NONE;
	}
};
USTRUCT()
struct GAMESYSTEM_API FGSActiveAbilitiesSlotConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 NumberOfSlots;
};

USTRUCT()
struct FGSAbilityBookConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 SlotNumber;

};