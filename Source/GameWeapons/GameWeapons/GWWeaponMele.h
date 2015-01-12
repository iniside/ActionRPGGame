#pragma once
#include "GWWeapon.h"
#include "GWWeaponMele.generated.h"

/*
	Base class for all weapons.
	Mele and ranged weapons need separate classes (at least).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEWEAPONS_API AGWWeaponMele : public AGWWeapon
{
	GENERATED_UCLASS_BODY()
};
