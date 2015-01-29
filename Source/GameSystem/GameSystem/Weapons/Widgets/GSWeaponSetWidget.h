#pragma once
#include "UserWidget.h"
#include "GSWeaponSetWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponSetWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon UI")
	TArray<class UGSWeaponSetSlotWidget*> WeaponSlots;
};
