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

	Invalid,
};