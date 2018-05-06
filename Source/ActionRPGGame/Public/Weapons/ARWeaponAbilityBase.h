// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ARAbilityBase.h"
#include "Effects/GAGameEffect.h"
#include "ARWeaponAbilityBase.generated.h"

class AARWeaponAvatar;
/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponAbilityBase : public UARAbilityBase
{
	GENERATED_BODY()
protected:

	/* Amount of ammunition to add to magazine */
	UPROPERTY(EditAnywhere, Category = "Weapon Reload Effects")
		FAFPropertytHandle AmmoToAdd;
	FGAEffectHandle AmmoToAddHandle;

	/* Amount of ammo to take from owner */
	UPROPERTY(EditAnywhere, Category = "Weapon Reload Effects")
		FAFPropertytHandle AmmoToTake;
	FGAEffectHandle AmmoToTakeHandle;

	/*
		Default damage effects used, when no upgrades are present for this weapon ability.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		TArray<FAFPropertytHandle> DefaultDamageEffects;

	/*
		Damage effects which are actually applied by this weapon. 
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		TArray<FAFPropertytHandle> DamageEffects;

	/*
		Handles to currently applied effects. Their order matches effects from DamageEffects array.
	*/
	TArray<FGAEffectHandle> AppliedEffectHandles;

	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|Weapon")
		class AARWeaponBase* WeaponActor;

	/*
		Weapon item to which this abiliy belongs.
	*/
	UPROPERTY(BlueprintReadOnly, Transient, Category = "ActionRPGGame|Weapon")
		class UARItemWeapon* WeaponItem;


	/*
		Upgrades for this ability.
	*/
	UPROPERTY(Transient)
		FAFEffectContainerSimple UpgradeContainer;

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

public:
	inline void SetWeaponItem(class UARItemWeapon* InItem)
	{
		WeaponItem = InItem;
	};

	virtual void SetAttributes(UGAAttributesBase* InAttributes) override;
	virtual class UGAAttributesBase* GetAttributes() const override;
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual void OnAbilityInited() override;
	virtual void OnAbilityInputReady() override;
	virtual void OnAvatarReady() override;

	void ReplaceDamageEffects(const TArray<FAFPropertytHandle>& InEffects);
	void AddDamageEffects(const TArray<FAFPropertytHandle>& InEffects);
	void ResetDamageEffects();

	/*
		Replaces current DamageEffects with the one provided as Paremeter.
	*/
	virtual void InstallAmmoType(TSubclassOf<UAFEffectSpecBase> AmmoType) {};

	virtual void InstallAmmoType(TSoftClassPtr<UAFEffectSpecBase> AmmoType) {};

	virtual void InstallMagazineUpgrade(TSubclassOf<UAFEffectSpecBase> MagazineUpgrade) {};
	/*
	*/
	virtual void OnAmmoTypeInstalled(TSubclassOf<UAFEffectSpecBase> AmmoType) {};


	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Weapon")
		void ApplyDamageEffect(UObject* Target, FAFFunctionModifier Modifier);

	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Weapon")
		void ReloadWeapon();

	void AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade);
	void AddMagazineUpgrade(TSubclassOf<class UARMagazineUpgradeEffect> InMagazineUpgrade, float UpgradeValue);
	void RemoveMagazineUpgrade();
protected:
	void AddUpgrade(FAFPropertytHandle& PropertyHandle
		, FGAEffectHandle& EffectHandle
		, FAFEffectSpecHandle& SpecHandle
		, class AARCharacter* Character
		, float UpgradeValue);
};
