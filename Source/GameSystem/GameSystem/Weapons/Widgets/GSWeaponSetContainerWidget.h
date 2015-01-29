#pragma once
#include "UserWidget.h"
#include "../GSWeaponEquipmentComponent.h"
#include "GSWeaponSetContainerWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponSetContainerWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	class UGSWeaponEquipmentComponent* WeaponEquipComp;

	UPROPERTY()
		TSubclassOf<class UGSWeaponSetWidget> SetWidgetClass;

	UPROPERTY()
		TSubclassOf<class UGSWeaponSetItemWidget> ItemWidgetClass;

	UPROPERTY()
		TSubclassOf<class UGSWeaponSetSlotWidget> SlotWidgetClass;

	FName DropSlottName;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon UI")
		TArray<class UGSWeaponSetWidget*> WeaponSets;
public:
	void InitializeContainer();

protected:
	UFUNCTION()
	void InitializeInventory();

	UFUNCTION()
		void UpdateWeaponSet();
};
