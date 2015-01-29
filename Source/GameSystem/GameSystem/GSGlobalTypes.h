#pragma once
#include "GameplayTagContainer.h"
#include "GSGlobalTypes.generated.h"


UENUM()
enum class EGSWeaponHand : uint8
{
	Left,
	Right,
	BothHands,

	Invalid,
};

UENUM()
enum class EGSWeaponWield : uint8
{
	OneHand,
	TwoHands,
	Either, //weapon can be either wielded in one hand or in two hands if second hand is free.

	Invalid,
};

UENUM()
enum class EGSWeaponType : uint8
{
	MainHand, //will be held in main hand, which is left (binds to left action slot)
	OffHand, //will map by default to right action slot
	Either, //will map to first available action slot. essentialy to where player drop it.

	Invalid,
};

UENUM(BlueprintType)
enum class EGSWeaponEquipState : uint8
{
	MainHand, //weapon is only equiped in left hand
	OffHand, //weapon is only equiped in right hand
	DualWield, //two weapons are equiped (one in left hand and one in right hand

	Invalid
};

UENUM(BlueprintType)
enum class EGSCharacterState: uint8
{
	Idle,
	Moving,
	InCombat,

	Invalid,
};