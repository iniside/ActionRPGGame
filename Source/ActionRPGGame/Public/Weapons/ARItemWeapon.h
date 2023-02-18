// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effects/GAGameEffect.h"
#include "ARMagazineUpgradeItem.h"
#include "IFTypes.h"
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
	UPROPERTY(EditAnywhere, SaveGame, Category = "Ability")
		FARGunAttributesItem GeneratedAttributes;

	UPROPERTY(EditAnywhere, Category = "Visual")
		TSoftClassPtr<class AARWeaponBase> Weapon;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector HolsteredPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator HolsteredRotation;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector EquipedPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator EquipedRotation;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Ability")
		UARWeaponAbilityBase* AbilityInstance;

	//possibly replace with struct containing identical fields.
	//so it can be easier serialized and saved.
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Ability")
		class UARMagazineUpgradeItem* MagazineModification;

	UPROPERTY(Transient)
		float MagazineUpgradeValue;

	UPROPERTY(Transient)
		FAFAbilitySpecHandle AbilityHandle;

	UARItemWeapon();

	void SetAbility(class UARWeaponAbilityBase* InAbility);

	void AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade);
	void OnMagazineUpdateAdded();

	UARMagazineUpgradeItem* RemoveMagazineUpgrade();

	bool SpawnAbility();

	virtual void OnServerItemLoaded() override;

	virtual void OnItemAdded(uint8 InIndex) override;
	virtual void OnItemRemoved(uint8 InIndex) override;

	virtual void OnServerItemAdded(uint8 InIndex) override;
	virtual void OnServerItemChanged(uint8 InIndex) override;
	virtual void OnServerItemRemoved(uint8 InIndex) override;

	virtual void OnItemAddedEquipment(uint8 InIndex) override;
	virtual void OnItemChangedEquipment(uint8 InIndex) override;
	virtual void OnItemRemovedEquipment(uint8 InIndex) override;

	virtual void OnServerItemAddedEquipment(uint8 InIndex) override;
	virtual void OnServerItemChangedEquipment(uint8 InIndex) override;
	virtual void OnServerItemRemovedEquipment(uint8 InIndex) override;

	virtual void ClientPostItemDeserializeFromJson();

	virtual TArray<FARItemTooltipData> GetTooltipData() override;

protected:
};
