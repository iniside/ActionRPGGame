// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARAbilityBase.h"
#include "ARUIWeaponEquipment.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIWeaponEquipment : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Input")
		FGameplayTag ShootInput;
	UPROPERTY(EditAnywhere, Category = "Input")
		FGameplayTag ReloadInput;
	/*
		Weapons which are currently equiped.
	*/
	TArray<TWeakObjectPtr<class UARAbilityBase>> Weapons;
	
	/*
		Weapon which is currently active. (equiped).
	*/
	TWeakObjectPtr<class UARAbilityBase> ActiveWeapon;
	int32 ActiveWeaponIndex;
public:	
	// Sets default values for this component's properties
	UARUIWeaponEquipment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void NativeEquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex);
	UFUNCTION()
		void OnWeaponReady(FGameplayTag InAbilityTag, int32 SlotIndex);
	UFUNCTION(BlueprintCallable, meta=(DisplayName = "Equip Weapon"), Category = "ActionRPGGame|UI|Weapon")
		void BP_EquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNextWeapon(int32 WeaponIndex);
	void ServerNextWeapon_Implementation(int32 WeaponIndex);
	bool ServerNextWeapon_Validate(int32 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientNextWeapon(int32 WeaponIndex);
	void ClientNextWeapon_Implementation(int32 WeaponIndex);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPreviousWeapon(int32 WeaponIndex);
	void ServerPreviousWeapon_Implementation(int32 WeaponIndex);
	bool ServerPreviousWeapon_Validate(int32 WeaponIndex);
};
