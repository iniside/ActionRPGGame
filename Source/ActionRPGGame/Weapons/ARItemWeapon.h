// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effects/GAGameEffect.h"
#include "ARItemBase.h"
#include "GameplayTags.h"
#include "ARItemWeapon.generated.h"

UENUM()
enum class EARWeaponUpgradeType : uint8
{
	Magazine
};

USTRUCT()
struct FARWeaponModInfo
{
	GENERATED_BODY();
public:
	UPROPERTY()
		EARWeaponUpgradeType UpgradeType;
	UPROPERTY()
		TSoftObjectPtr<UTexture2D> Icon;
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
		TSoftClassPtr<class UARMagazineUpgradeItem> MagazineModification;

	/*
		So it will actually actor as mini inventory with predefined slots.
		That way we should be able to generate mods on the fly and store them as json in external database.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
		class UARMagazineUpgradeItem* MagazineModificationObj;

	FAFPropertytHandle MagazineEffect;
	FGAEffectHandle MagazineEffectHandle;

	void AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade);
	void OnMagazineUpdateAdded();

	UFUNCTION(Client, Reliable)
		void ClientOnMagazineAdded(const FARWeaponModInfo& ModInfo);
	void ClientOnMagazineAdded_Implementation(const FARWeaponModInfo& ModInfo);
	
	TSoftClassPtr<class UARMagazineUpgradeItem> RemoveMagazineUpgrade();

	virtual void OnItemAdded(uint8 LocalIndex) override;
	virtual void OnItemRemoved(uint8 LocalIndex) override;
};
