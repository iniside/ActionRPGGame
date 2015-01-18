#pragma once
#include "UserWidget.h"
#include "GSWeaponInfoWidget.generated.h"
/*
	Base class to show abilities in UI.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponInfoWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Info")
		int32 GetCurrentMagazineAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Info")
		int32 GetCurrentTotalAmmo() const;

	UPROPERTY()
	class UGSItemWeaponInfo* CurrentWeapon;
};
