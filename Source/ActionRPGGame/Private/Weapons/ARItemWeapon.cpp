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

void UARItemWeapon::SetAbility(class UARWeaponAbilityBase* InAbility)
{
	AbilityInstance = InAbility;
	AbilityInstance->SetWeaponItem(this);
	if (Attributes)
	{
		if (AbilityInstance)
		{
			AbilityInstance->GetAttributes()->CopyFromOtherAttributes(Attributes);
		}
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

void UARItemWeapon::SpawnAbility()
{
	UIFInventoryComponent* InventoryComp = Cast<UIFInventoryComponent>(GetOuter());
	if (!InventoryComp)
		return;

	AARPlayerController* PC = Cast<AARPlayerController>(InventoryComp->GetOwner());
	if (!PC)
		return;

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());

	if (!Character)
		return;

	TSubclassOf<UARWeaponAbilityBase> ABClass = Ability.LoadSynchronous();
	if (ABClass)
	{
		AbilityInstance = NewObject<UARWeaponAbilityBase>(Character, ABClass);
		AbilityInstance->GetAttributes()->CopyFromOtherAttributes(Attributes);
	}
}

void UARItemWeapon::OnItemAdded(uint8 LocalIndex)
{
	SpawnAbility();
}
void UARItemWeapon::OnItemRemoved(uint8 LocalIndex)
{

}
void UARItemWeapon::OnServerItemAdded(uint8 LocalIndex)
{
	SpawnAbility();
}
void UARItemWeapon::OnServerItemChanged(uint8 LocalIndex)
{
}
void UARItemWeapon::OnServerItemRemoved(uint8 LocalIndex)
{
}

void UARItemWeapon::OnItemAddedEquipment(uint8 LocalIndex) 
{

};
void UARItemWeapon::OnItemChangedEquipment(uint8 LocalIndex) 
{
};
void UARItemWeapon::OnItemRemovedEquipment(uint8 LocalIndex) 
{
};

void UARItemWeapon::OnServerItemAddedEquipment(uint8 LocalIndex) 
{
};
void UARItemWeapon::OnServerItemChangedEquipment(uint8 LocalIndex) 
{
};
void UARItemWeapon::OnServerItemRemovedEquipment(uint8 LocalIndex) 
{
};

void UARItemWeapon::PostItemLoad()
{

}
TSharedPtr<FJsonObject> UARItemWeapon::SaveToJson()
{
	typedef TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriter;
	typedef TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriterFactory;

	TSharedPtr<FJsonObject> UObj = MakeShareable(new FJsonObject());
	
	FIFJsonSerializer::UObjectToJsonObject(GetClass(), this, UObj.ToSharedRef());
	

	return UObj;
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