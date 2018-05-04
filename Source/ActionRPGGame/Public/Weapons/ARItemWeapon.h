// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effects/GAGameEffect.h"
#include "ARMagazineUpgradeItem.h"

#include "GameplayTags.h"
#include "ARItemWeapon.generated.h"

UENUM()
enum class EARWeaponUpgradeType : uint8
{
	Magazine
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ACTIONRPGGAME_API UARItemWeapon : public UARItemBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Ability")
		TSoftClassPtr<class UARWeaponAbilityBase> Ability;
	
	UPROPERTY(EditAnywhere, Category = "Visual")
		TSoftClassPtr<class AARWeaponBase> Weapon;


	/* 
		Values from these attributes will be copied to ability, after ability is instanced.
		It's here to allow random generation and easily store that information in Database instead of storing ability.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Attributes")
		class UARGunAttributes* Attributes;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector HolsteredPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator HolsteredRotation;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector EquipedPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator EquipedRotation;

	UPROPERTY(EditAnywhere, Category = "Ability")
		UARWeaponAbilityBase* AbilityInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
		class UARMagazineUpgradeItem* MagazineModification;

	UPROPERTY(Transient)
		float MagazineUpgradeValue;

	void SetAbility(class UARWeaponAbilityBase* InAbility);

	void AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade);
	void OnMagazineUpdateAdded();

	UARMagazineUpgradeItem* RemoveMagazineUpgrade();

	virtual void OnItemAdded(uint8 LocalIndex) override;
	virtual void OnItemRemoved(uint8 LocalIndex) override;

	virtual void OnItemAddedEquipment(uint8 LocalIndex);
	virtual void OnItemChangedEquipment(uint8 LocalIndex);
	virtual void OnItemRemovedEquipment(uint8 LocalIndex);

	virtual void OnServerItemAddedEquipment(uint8 LocalIndex);
	virtual void OnServerItemChangedEquipment(uint8 LocalIndex);
	virtual void OnServerItemRemovedEquipment(uint8 LocalIndex);

	virtual void PostItemLoad();

	virtual TArray<FARItemTooltipData> GetTooltipData();
protected:
};
