#pragma once
#include "GISItemData.h"
#include "GSItemTypes.h"
#include "GSItemInfo.h"
#include "../GSWeaponEquipmentTypes.h"
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

	inline void SetIsWeaponReady(bool bIsWeaponReadyIn);
	inline bool GetIsWeaponReady();
	virtual void SetEquipingTime(float TimeIn);
	virtual UTexture2D* GetImage() override;

	virtual AActor* GetActorToAttach();
	virtual const TArray<FGSWeaponSocketInfo>& GetPossibleWeaponSockets() const;
	virtual const EGSWeaponWield GetWeaponWield() const;
	
	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;


	virtual bool InputPressed_Implementation() override;
	virtual bool InputReleased_Implementation() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 30;

	virtual int32 GetItemTypeID() const override { return UGSItemWeaponInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) override { return UGSItemWeaponInfo::ItemTypeID == ItemTypeIDIn; }

	virtual void EquipWeapon();

	EGSWeaponHand CurrentHand;

protected:
	TArray<FGSWeaponSocketInfo> socketArray;


	/*
		move equiping logic here in form of interface (or not ?)
		1. this inventory info class know about what weapon it contains.
		2. so we have to check what are currently equiped weapons.
		3. Then based on types of currently equiped weapons, and hand we want to equip this weapon
		4. We can decide what action should be taken. (ie just equip weapon,
		unequip weapon from one hand, unequip weapon, from other hand (if it is two handed))
		etc.
	*/
};
