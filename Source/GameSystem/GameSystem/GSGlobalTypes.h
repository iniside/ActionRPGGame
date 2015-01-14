#pragma once
#include "GameplayTagContainer.h"
#include "GSGlobalTypes.generated.h"


UENUM()
enum class EGSWeaponHand : uint8
{
	Left,
	Right,

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