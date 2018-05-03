// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIInventoryComponent.h"
#include "ARPlayerController.h"
#include "ARCharacter.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARMagazineUpgradeItem.h"

#include "UI/ARHUD.h"
#include "UI/Inventory/ARItemView.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/Weapons/ARListItemWeaponWidget.h"
#include "UI/Inventory/Weapons/Modifications/ARItemMagazineView.h"
#include "UI/Inventory/Weapons/Modifications/ARListItemMagazineView.h"
#include "UI/Inventory/Weapons/ARWeaponModificationView.h"

// Sets default values for this component's properties
UARUIInventoryComponent::UARUIInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARUIInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponModificationViewClass)
	{
		if (AARHUD* HUD = Cast<AARHUD>(GetOwner()))
		{
			if (AARPlayerController* PC = Cast<AARPlayerController>(HUD->GetOwningPlayerController()))
			{
				WeaponModificationView = CreateWidget<UARWeaponModificationView>(PC, WeaponModificationViewClass);

				WeaponModificationView->SetVisibility(ESlateVisibility::Collapsed);
				WeaponModificationView->AddToViewport();
			}
		}	
	}
}


// Called every frame
void UARUIInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARUIInventoryComponent::CreateInventoryView(AARPlayerController* PC)
{
	if (InventoryViewClass)
	{
		InventoryView = CreateWidget<UARInventoryScreenWidget>(PC, InventoryViewClass);

		InventoryView->SetVisibility(ESlateVisibility::Collapsed);
		InventoryView->Inventory = this;
		InventoryView->AddToViewport();
	}
}

void UARUIInventoryComponent::ShowHideInventory()
{
	if (InventoryView->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InventoryView->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InventoryView->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UARUIInventoryComponent::ShowWeaponsForSlot(class UARItemView* ForSlot)
{
	SelectedWeapon = ForSlot->LocalIndex;
	TArray<uint8> Items;
	AARPlayerController* PC = nullptr;
	if (AARHUD* HUD = Cast<AARHUD>(GetOwner()))
	{
		PC = Cast<AARPlayerController>(HUD->PlayerOwner);
		if (PC)
		{
			Items = PC->MainInventory->GetLocalItemIdxs(UARItemWeapon::StaticClass());
		}
		if (AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn()))
		{
			UARItemWeapon* Item = Character->WeaponInventory->GetItem<UARItemWeapon>(ForSlot->LocalIndex);
			ModifiedWeapon = Item;
			if (Item)
			{
				InventoryView->SetWeaponName(Item->GetName());
			}
		}
	}

	InventoryView->UpdateItemList<UARItemWeapon, UARListItemWeaponWidget>(Items, ListItemWeaponClass, PC, ForSlot);
}

void UARUIInventoryComponent::RemoveWeaponFromSlot(int8 Index)
{
	AARPlayerController* PC = nullptr;
	AARCharacter* Character = nullptr;
	if (AARHUD* HUD = Cast<AARHUD>(GetOwner()))
	{
		PC = Cast<AARPlayerController>(HUD->PlayerOwner);

		if (!PC)
			return;
		Character = Cast<AARCharacter>(PC->GetPawn());
		if (!Character)
			return;

		PC->MainInventory->AddItemFromEquipmentAnySlot(Character->WeaponInventory, Index);

	}
}

void UARUIInventoryComponent::AddWeaponToSlot(uint8 TargetNetIndex
	, uint8 TargetLocalIndex
	, uint8 SourceNetIndex
	, uint8 SourceLocalIndex)
{
	AARHUD* HUD = Cast<AARHUD>(GetOwner());
	if (!HUD)
		return;
	AARPlayerController* PC = Cast<AARPlayerController>(HUD->PlayerOwner);
	if (!PC)
		return;
	
	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	if (!Character)
		return;

	Character->WeaponInventory->AddItemFromInventory(PC->MainInventory, SourceNetIndex, TargetNetIndex);
}

void UARUIInventoryComponent::UnequipWeaponFromSlot(uint8 SourceNetIndex, uint8 SourceLocalIndex)
{
	AARHUD* HUD = Cast<AARHUD>(GetOwner());
	if (!HUD)
		return;
	AARPlayerController* PC = Cast<AARPlayerController>(HUD->PlayerOwner);
	if (!PC)
		return;

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	if (!Character)
		return;

	Character->WeaponInventory->Unequip(SourceLocalIndex);
	
	if (ModifiedWeapon.IsValid())
	{
		ModifiedWeapon.Reset();
	}
}

void UARUIInventoryComponent::ShowUpgradesForWeapon(class UARItemMagazineView* For)
{
	TArray<uint8> Items;
	AARPlayerController* PC = nullptr;
	if (AARHUD* HUD = Cast<AARHUD>(GetOwner()))
	{
		PC = Cast<AARPlayerController>(HUD->PlayerOwner);
		if (PC)
		{
			Items = PC->MainInventory->GetLocalItemIdxs(UARMagazineUpgradeItem::StaticClass());
		}
	}
	
	InventoryView->AddWeaponMods<UARMagazineUpgradeItem, UARListItemMagazineView>(Items, ListItemMagazinelass, PC, For);
}

void UARUIInventoryComponent::ModifyWeapon()
{
	AARHUD* HUD = Cast<AARHUD>(GetOwner());
	if (!HUD)
		return;
	AARPlayerController* PC = Cast<AARPlayerController>(HUD->PlayerOwner);
	if (!PC)
		return;

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	if (!Character)
		return;

	UARItemWeapon* Weapon = Character->WeaponInventory->GetItem<UARItemWeapon>(SelectedWeapon);
	ModifiedWeapon = Weapon;
	if (!Weapon)
		return;

	//WeaponModificationView->StartModifyWeapon(Weapon);

	if (Weapon->MagazineModification)
	{
		InventoryView->MagazineUpgrade->OnItemChanged(0, 0, Weapon->MagazineModification);
	}
}

void UARUIInventoryComponent::AddMagazineUpgrade(uint8 SourceNetIndex, uint8 SourceLocalIndex)
{
	//that's prototype. We should handle entire thing on server, either directly on item weapon, or trough WeaponInventory.
	AARHUD* HUD = Cast<AARHUD>(GetOwner());
	if (!HUD)
		return;
	AARPlayerController* PC = Cast<AARPlayerController>(HUD->PlayerOwner);
	if (!PC)
		return;

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	if (!Character)
		return;

	UARWeaponInventoryComponent* WeaponInventory = Character->WeaponInventory;

	WeaponInventory->AddMagazineMod(SelectedWeapon, SourceLocalIndex);
}

void UARUIInventoryComponent::RemoveMagazineUpgrade()
{
	AARHUD* HUD = Cast<AARHUD>(GetOwner());
	if (!HUD)
		return;
	AARPlayerController* PC = Cast<AARPlayerController>(HUD->PlayerOwner);
	if (!PC)
		return;

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	if (!Character)
		return;

	UARWeaponInventoryComponent* WeaponInventory = Character->WeaponInventory;
	WeaponInventory->RemoveMagazineMod(SelectedWeapon);
}