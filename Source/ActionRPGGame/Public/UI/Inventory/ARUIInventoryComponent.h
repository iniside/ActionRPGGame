// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARUIInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARInventoryScreenWidget> InventoryViewClass;
	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARListItemWeaponWidget> ListItemWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARListItemMagazineView> ListItemMagazinelass;

	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARWeaponModificationView> WeaponModificationViewClass;
public:
	UPROPERTY(EditAnywhere, Category = "Views")
		TSubclassOf<class UARItemTooltipView> WeaponItemTooltipViewClass;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|UI|Inventory")
		class UARInventoryScreenWidget* InventoryView;

	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|UI|Inventory")
		class UARWeaponModificationView* WeaponModificationView;

	/* NetIndex of selected Weapon. */
	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|UI|Inventory")
		uint8 SelectedWeapon;

	TWeakObjectPtr<class UARItemWeapon> ModifiedWeapon;

public:	
	// Sets default values for this component's properties
	UARUIInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	inline UARInventoryScreenWidget* GetInventoryView() { return InventoryView; }
	void CreateInventoryView(AARPlayerController* PC);
	void ShowHideInventory();

public:
	void ShowWeaponsForSlot(class UARItemView* ForSlot);
	void RemoveWeaponFromSlot(int8 Index);
	void AddWeaponToSlot(uint8 TargetNetIndex
		, uint8 TargetLocalIndex
		, uint8 SourceNetIndex
		, uint8 SourceLocalIndex);

	void UnequipWeaponFromSlot(uint8 SourceNetIndex, uint8 SourceLocalIndex);

	void ShowUpgradesForWeapon(class UARItemMagazineView* For);

	void ModifyWeapon();
	void AddMagazineUpgrade(uint8 SourceNetIndex, uint8 SourceLocalIndex);
	void RemoveMagazineUpgrade();
};
