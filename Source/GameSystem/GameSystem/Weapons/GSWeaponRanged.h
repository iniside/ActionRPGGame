#pragma once
#include "GWWeaponRanged.h"
#include "../GSWeaponEquipmentTypes.h"
#include "GSWeaponRanged.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSWeaponRanged : public AGWWeaponRanged
{
	GENERATED_UCLASS_BODY()
public:
	/**
	 *	List of possible sockets, to which this weapon can attach itself when equiped.
	 */
	UPROPERTY(EditAnywhere, Category = "Socket Attachments")
		TArray<FGSWeaponSocketInfo> SocketList;
};
