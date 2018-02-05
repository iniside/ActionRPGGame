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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
		TArray<TSubclassOf<class AARWeaponBase>> WeaponClasses;

	//currently equipped weapons
	UPROPERTY()
		TArray<class AARWeaponBase*> Weapons;

	//currently active weapons.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentWeapon, Category = "Weapon Manager")
		class AARWeaponBase* CurrentWeapon;
	UFUNCTION()
		void OnRep_CurrentWeapon();

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

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
		void AddToWeaponInventory(TSubclassOf<class AARWeaponBase> InWeapon);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Add Weapon To Manager"), Category = "Weapon Manager")
		void BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);

	virtual void AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);
	virtual void ServerAddWeaponToManager_Implementation(EAMGroup Group, EAMSlot Slot, int32 Idx);
	bool ServerAddWeaponToManager_Validate(EAMGroup Group, EAMSlot Slot, int32 Idx);

	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	UFUNCTION()
		void OnWeaponInputRead(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags);

	void OnWeaponAbilityReady(const FGameplayTag& WeaponAbility, EAMGroup InGroup);

	UFUNCTION()
		void ClientOnWeaponAbilityReady(FGameplayTag WeaponAbility, EAMGroup InGroup);

protected:
	virtual void OnNextGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;
	virtual void OnPreviousGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;

	void EquipWeapon(const FGameplayTag& PreviousWeaponTag, const FGameplayTag& NextWeaponTag);

	FGameplayTag FindNextValid();
	FGameplayTag FindPreviousValid();
};
