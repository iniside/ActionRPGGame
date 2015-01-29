#pragma once
#include "GISItemData.h"
#include "GSItemTypes.h"
#include "GSItemInfo.h"
#include "../GSWeaponEquipmentTypes.h"
#include "../GSGlobalTypes.h"
#include "GSItemWeaponInfo.h"
#include "GSItemWeaponRangedInfo.generated.h"
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


UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSItemWeaponRangedInfo : public UGSItemWeaponInfo
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Item")
		TSubclassOf<class AGSWeaponRanged> RangedWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Weapon Parts")
		TSubclassOf<class UGWAmmo> WeaponAmmoClass;

	UPROPERTY(Replicated)
		class AGSWeaponRanged* RangedWeapon;

	UPROPERTY()
		int32 RemainingAmmoMagazine;
	UPROPERTY()
		int32 RemainingAmmoTotal;

	virtual int32 GetCurrentMagainzeAmmo();
	virtual int32 GetCurrentTotalAmmo();
	virtual void SetEquipingTime(float TimeIn) override;
	virtual UTexture2D* GetImage() override;
	virtual AActor* GetActorToAttach() override;
	virtual const TArray<FGSWeaponSocketInfo>& GetPossibleWeaponSockets() const override;
	virtual const EGSWeaponWield GetWeaponWield() const override;
	virtual const EGSWeaponType GetWeaponType() const override;
	virtual FVector GetCrosshairStartLocation() override;

	virtual UAnimSequence* GetEquipedAnimation() override;

	virtual UAnimSequence* GetIdleAnimation() override;
	virtual UAnimSequence* GetMoveAnimation() override;
	virtual UAnimSequence* GetCombatAnimation() override;

	virtual const float GetCurrentHorizontalRecoil() const override;
	virtual const float GetCurrentVerticalRecoil() const override;

	virtual FVector GetCrosshairTraceStartLocation() override;

	virtual UAimOffsetBlendSpace* GetEquipedAimBlendSpace() override;
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;

	virtual bool InputPressed_Implementation() override;
	virtual bool InputReleased_Implementation() override;
	virtual void InputRealadWeapon();
};
