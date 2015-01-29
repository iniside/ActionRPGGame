#pragma once
#include "UserWidget.h"
#include "../GSWeaponEquipmentComponent.h"
#include "GSWeaponSetItemWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponSetItemWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	class UGSWeaponEquipmentComponent* WeaponEquipComp;
public:
	UPROPERTY()
		FGSWeaponSetSlot WeaponSlot;
	int32 ItemSlotIndex;
	int32 ItemTabIndex;
public:
	UFUNCTION(BlueprintPure, Category = "Weapon UI")
		UTexture2D* GetIcon() const;
};
