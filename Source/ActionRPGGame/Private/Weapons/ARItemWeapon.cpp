// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeapon.h"

#include "Effects/GABlueprintLibrary.h"
#include "IFInventoryComponent.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"
#include "Attributes/ARGunAttributes.h"
#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/Weapons/Modifications/ARItemMagazineView.h"

#include "Weapons/ARWeaponAbilityBase.h"
#include "Weapons/ARMagazineUpgradeItem.h"
#include "Weapons/ARWeaponInventoryComponent.h"
#include "Weapons/ARMagazineUpgradeEffect.h"

UARItemWeapon::UARItemWeapon()
{
	AbilityHandle = FAFAbilitySpecHandle::GenerateHandle();
}

void UARItemWeapon::SetAbility(class UARWeaponAbilityBase* InAbility)
{
	AbilityInstance = InAbility;
	AbilityInstance->SetWeaponItem(this);

	if (AbilityInstance)
	{
		AbilityInstance->GetAttributes()->CopyFromStruct(FARGunAttributesItem::StaticStruct(), &GeneratedAttributes);
	}
	
}
void UARItemWeapon::AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade)
{
	if (!AbilityInstance)
	{
		return; //add log.
	}

	UARWeaponInventoryComponent* WeaponComponent = Cast<UARWeaponInventoryComponent>(GetOuter());
	//might also check for PC inventory.

	if (!WeaponComponent)
		return;

	AARCharacter* Character = Cast<AARCharacter>(WeaponComponent->GetOwner());
	if (!Character)
		return;

	MagazineModification = DuplicateObject<UARMagazineUpgradeItem>(InMagazineUpgrade, this);

	//this part only on server.
	if (Character->Role >= ENetRole::ROLE_Authority)
	{
		MagazineUpgradeValue = InMagazineUpgrade->MagazineUpgradeValue;
		AbilityInstance->AddMagazineUpgrade(InMagazineUpgrade->UpgradeEffect, MagazineUpgradeValue);
	}
	OnMagazineUpdateAdded();
}
void UARItemWeapon::OnMagazineUpdateAdded()
{
}

UARMagazineUpgradeItem* UARItemWeapon::RemoveMagazineUpgrade()
{
	AbilityInstance->RemoveMagazineUpgrade();

	return MagazineModification;
}

bool UARItemWeapon::SpawnAbility()
{
	bool bSpawned = false;

	if (AbilityInstance)
		return true;

	AARCharacter* Character = nullptr;
	if (UIFInventoryComponent* InventoryComp = Cast<UIFInventoryComponent>(GetOuter()))
	{
		AARPlayerController* PC = Cast<AARPlayerController>(InventoryComp->GetOwner());
		if (!PC)
			return bSpawned;

		Character = Cast<AARCharacter>(PC->GetPawn());
	}
	else if(UIFEquipmentComponent* EquipComp = Cast<UIFEquipmentComponent>(GetOuter()))
	{
		Character = Cast<AARCharacter>(EquipComp->GetOwner());
	}
	
	if (!Character)
		return bSpawned;

	TSubclassOf<UARWeaponAbilityBase> ABClass = Ability.LoadSynchronous();
	if (ABClass)
	{
		AbilityInstance = NewObject<UARWeaponAbilityBase>(Character, ABClass);
		AbilityInstance->GetAttributes()->CopyFromStruct(FARGunAttributesItem::StaticStruct(), &GeneratedAttributes);
		if (MagazineModification)
		{
			AbilityInstance->AddMagazineUpgrade(MagazineModification);
		}
		bSpawned = true;
	}

	return bSpawned;
}
void UARItemWeapon::OnServerItemLoaded()
{
	/*
		1. Generate Weapon Stats here.
		2. Add random perks (weapon)
		3. Add random attributes to give (Character).
		4. Add random effects to give (Character).
	*/
}
void UARItemWeapon::OnItemAdded(uint8 InIndex)
{
	SpawnAbility();
}
void UARItemWeapon::OnItemRemoved(uint8 InIndex)
{

}
void UARItemWeapon::OnServerItemAdded(uint8 InIndex)
{
	SpawnAbility();
}
void UARItemWeapon::OnServerItemChanged(uint8 InIndex)
{
}
void UARItemWeapon::OnServerItemRemoved(uint8 InIndex)
{
}

void UARItemWeapon::OnItemAddedEquipment(uint8 InIndex) 
{
};
void UARItemWeapon::OnItemChangedEquipment(uint8 InIndex) 
{
};
void UARItemWeapon::OnItemRemovedEquipment(uint8 InIndex) 
{
};

void UARItemWeapon::OnServerItemAddedEquipment(uint8 InIndex) 
{
	UARWeaponInventoryComponent* WeaponComponent = Cast<UARWeaponInventoryComponent>(GetOuter());
	if (!WeaponComponent)
		return;

};
void UARItemWeapon::OnServerItemChangedEquipment(uint8 InIndex) 
{
};
void UARItemWeapon::OnServerItemRemovedEquipment(uint8 InIndex) 
{
};

void UARItemWeapon::ClientPostItemDeserializeFromJson()
{
}

TArray<FARItemTooltipData> UARItemWeapon::GetTooltipData()
{
	TArray<FARItemTooltipData> Data;

	FARItemTooltipData ItemName("ItemName", GetName());
	Data.Add(ItemName);

	if (!AbilityInstance)
		return Data;

	UARGunAttributes* ABAttr = AbilityInstance->GetAttributesTyped<UARGunAttributes>();

	if (ABAttr)
	{
		FARItemTooltipData BaseDamage("BaseDamage", FString::SanitizeFloat(ABAttr->BaseDamage.GetCurrentValue()));
		Data.Add(BaseDamage);

		FARItemTooltipData CritChance("CritChance", FString::SanitizeFloat(ABAttr->CritChance.GetCurrentValue()));
		Data.Add(CritChance);

		FARItemTooltipData Magazine("Magazine", FString::SanitizeFloat(ABAttr->Magazine.GetCurrentValue()));
		Data.Add(Magazine);

		FARItemTooltipData RateOfFire("RateOfFire", FString::SanitizeFloat(ABAttr->RateOfFire.GetCurrentValue()));
		Data.Add(RateOfFire);

		FARItemTooltipData ReloadSpeed("ReloadSpeed", FString::SanitizeFloat(ABAttr->ReloadSpeed.GetCurrentValue()));
		Data.Add(ReloadSpeed);

		FARItemTooltipData HorizontalStability("HorizontalStability", FString::SanitizeFloat(ABAttr->HorizontalStability.GetCurrentValue()));
		Data.Add(HorizontalStability);

		FARItemTooltipData VerticalStability("VerticalStability", FString::SanitizeFloat(ABAttr->VerticalStability.GetCurrentValue()));
		Data.Add(VerticalStability);

		FARItemTooltipData Spread("Spread", FString::SanitizeFloat(ABAttr->Spread.GetCurrentValue()));
		Data.Add(Spread);

	}

	return Data;
}