#pragma once
#include "GISItemData.h"
#include "GSItemTypes.h"
#include "GSItemInfo.h"
#include "../GSGlobalTypes.h"

#include "GSItemWeaponInfo.generated.h"
/*
	1. Should I add attributes here ?
	2. Should this be purerly inventory class ?
	3. Or should it contain information about actuall item ? Like attributes,
	meshes, references to other items (for composite items, which are assmbled from various
	componenets).
*/
/*
	Temporary.
*/


UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSItemWeaponInfo : public UGSItemInfo
{
	GENERATED_UCLASS_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category = "Item")
		EGSWeaponWield WeaponWieldType;
	/*
		Weapon Class.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Item")
		TSubclassOf<class AGSWeaponRanged> Weapon;
	
	/*
		Spawned When weapon is equiped into weapon slot.
	*/
	UPROPERTY(Replicated)
	class AGSWeaponRanged* ActiveWeapon;

	UPROPERTY()
		FName LastAttachedSocket;

	virtual UTexture2D* GetImage() override;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;


	virtual bool InputPressed_Implementation() override;
	virtual bool InputReleased_Implementation() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 30;

	virtual int32 GetItemTypeID() const override { return UGSItemWeaponInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) override { return UGSItemWeaponInfo::ItemTypeID == ItemTypeIDIn; }
};
