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
//public:
//	UPROPERTY(EditAnywhere, Category = "Input")
//		FGameplayTag ShootInput;
//	UPROPERTY(EditAnywhere, Category = "Input")
//		FGameplayTag ReloadInput;
//
//	TArray<TWeakObjectPtr<UARWeaponAbilityBase>> AllWeapons;
//
//	TMap<FGameplayTag, TWeakObjectPtr<UARWeaponAbilityBase>> AllWeapons2;
//	//Maps weapon slot to delegate to broadcast that weapon changed in given slot.
//	TMap<FGameplayTag, FAROnWeaponReady> OnWeaponChanged;
//	TWeakObjectPtr<UARWeaponAbilityBase> CurrentWeapon;
//
//	UPROPERTY()
//		EARWeaponSlot ActiveWeaponIndex;
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
	//void AddOnWeaponChangedEvent(const FGameplayTag& InTag, FAROnWeaponReady& InDelegate);
	//void SendOnWeaponChangedEvent(const FGameplayTag& InTag, UARWeaponAbilityBase* InAbility);
	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	//UFUNCTION(Server, Reliable, WithValidation)
	//	void ServerNextWeapon(int32 WeaponIndex);
	//void ServerNextWeapon_Implementation(int32 WeaponIndex);
	//bool ServerNextWeapon_Validate(int32 WeaponIndex);
	//UFUNCTION(Client, Reliable)
	//	void ClientNextWeapon(int32 WeaponIndex);
	//void ClientNextWeapon_Implementation(int32 WeaponIndex);

	//UFUNCTION(Server, Reliable, WithValidation)
	//	void ServerPreviousWeapon(int32 WeaponIndex);
	//void ServerPreviousWeapon_Implementation(int32 WeaponIndex);
	//bool ServerPreviousWeapon_Validate(int32 WeaponIndex);
	//
	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Equip Weapon"), Category = "ActionRPGGame|UI|Weapon")
	//	void BP_EquipWeapon(const FGameplayTag& InAbilityTag, EARWeaponSlot SlotIndex);

	//void NativeEquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex);
	//UFUNCTION()
	//	void OnWeaponReady(FGameplayTag InAbilityTag, int32 SlotIndex);
};
