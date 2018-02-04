// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "ARWeaponsTypes.h"
#include "AMAbilityManagerComponent.h"
#include "ARWeaponManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FAROnWeaponReady, class UARWeaponAbilityBase*);


/* Add On Character. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARWeaponManagerComponent : public UAMAbilityManagerComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Weapons")
		TArray<TSubclassOf<class AARWeaponBase>> WeaponClasses;

	//currently equipped weapons
	UPROPERTY()
		TArray<class AARWeaponBase*> Weapons;

	//currently active weapons.
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Manager")
		class AARWeaponBase* CurrentWeapon;

	TMap<FGameplayTag, AARWeaponBase*> AbilityToWeapon;
public:	
	// Sets default values for this component's properties
	UARWeaponManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UGAAbilityBase* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Equip Weapon"), Category = "Weapon Manager")
		void BP_EquipWeapon(EAMGroup Group, EAMSlot Slot, int32 Idx);

	virtual void EquipWeapon(EAMGroup Group, EAMSlot Slot, int32 Idx);

	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	UFUNCTION()
	void OnWeaponInputRead(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags);
};
