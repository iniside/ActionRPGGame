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
	Mele,
	Ranged,

	Invalid,
};

UENUM(BlueprintType)
enum class EGSWeaponEquipState : uint8
{
	LeftHand, //weapon is only equiped in left hand
	RightHand, //weapon is only equiped in right hand
	BothHands, //one weapon takes both hand
	DualWield, //two weapons are equiped (one in left hand and one in right hand

	Invalid
};