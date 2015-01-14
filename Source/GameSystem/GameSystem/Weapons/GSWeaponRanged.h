#pragma once
#include "GWWeaponRanged.h"
#include "../GSWeaponEquipmentTypes.h"
#include "../GSGlobalTypes.h"
#include "GSWeaponRanged.generated.h"
/*
	Add interface IWeaponEquip, which would let each weapon decide
	how it want to be equiped
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSWeaponRanged : public AGWWeaponRanged
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "UI")
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
		EGSWeaponWield WieldType;
	/**
	 *	List of possible sockets, to which this weapon can attach itself when equiped.
	 */
	UPROPERTY(EditAnywhere, Category = "Socket Attachments")
		TArray<FGSWeaponSocketInfo> SocketList;

	/*
		move equiping logic here in form of interface
		1. Weapon know of it's type.
		2. We have to check what are currently equiped weapons.
		3. Weapon must know in what hand it currently is.
	*/
};
