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


USTRUCT()
struct FARItemWeaponData : public FARItemBaseData
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
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ACTIONRPGGAME_API UARItemWeapon : public UARItemBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FARItemWeaponData Data;

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
		FAFPropertytHandle MagazineUpgradeProperty;
	UPROPERTY(Transient)
		FGAEffectHandle MagazineEffectHandle;
	UPROPERTY(Transient)
		FAFEffectSpecHandle MagazineSpecHandle;

	UPROPERTY(Transient)
		FAFPropertytHandle BarrelUpgradeProperty;
	UPROPERTY(Transient)
		FGAEffectHandle BarrelEffectHandle;
	UPROPERTY(Transient)
		FAFEffectSpecHandle BarrelSpecHandle;

	UPROPERTY(Transient)
		FAFPropertytHandle ScopeUpgradeProperty;
	UPROPERTY(Transient)
		FGAEffectHandle ScopeEffectHandle;
	UPROPERTY(Transient)
		FAFEffectSpecHandle ScopeSpecHandle;

	UPROPERTY(Transient)
		float MagazineUpgradeValue;

	/*
		So it will actually actor as mini inventory with predefined slots.
		That way we should be able to generate mods on the fly and store them as json in external database.
	*/

	UPROPERTY() //replicated
		FARMagazineUpgradeItemData MagazineUpgrade;

	void AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade);
	void OnMagazineUpdateAdded();

	void AddUpgrade(FAFPropertytHandle& PropertyHandle
		, FGAEffectHandle& EffectHandle
		, FAFEffectSpecHandle& SpecHandle
		, class AARCharacter* Character
		, float UpgradeValue);

	
	UARMagazineUpgradeItem* RemoveMagazineUpgrade();

	virtual void OnItemAdded(uint8 LocalIndex) override;
	virtual void OnItemRemoved(uint8 LocalIndex) override;

	virtual void PostItemLoad();
};
