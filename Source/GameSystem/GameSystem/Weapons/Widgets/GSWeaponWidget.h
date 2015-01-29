#pragma once
#include "UserWidget.h"
#include "GSWeaponWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	class UGSItemWeaponInfo* CurrentWeapon;

	UFUNCTION(BlueprintPure, Category = "Weapon UI")
		UTexture2D* GetWeaponIcon() const;
};
