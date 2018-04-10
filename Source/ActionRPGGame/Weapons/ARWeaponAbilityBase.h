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

public:
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
};
