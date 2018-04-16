// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "ARWeaponsTypes.h"
#include "AMAbilityManagerComponent.h"
#include "ARWeaponManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FAROnWeaponReady, class UARWeaponAbilityBase*);

USTRUCT(BlueprintType)
struct FARWeaponAttachment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		EAMGroup Group;
	
	UPROPERTY(EditAnywhere)
		FName SocketName;
	

	bool operator==(int32 InGroup) const
	{
		return Group == AMIntToEnum<EAMGroup>(InGroup);
	}

	bool operator==(const FARWeaponAttachment& Other) const
	{
		return Group == Other.Group;
	}
	bool operator==(EAMGroup InGroup) const
	{
		return Group == InGroup;
	}
};
/* Add On Character. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARWeaponManagerComponent : public UAMAbilityManagerComponent
{
	GENERATED_BODY()
protected:
	static constexpr int32 MAX_WEAPONS = 4; //maximum weapon + empty hands
public:
	UPROPERTY(EditAnywhere, Category = "Attachment Config")
		FName EquipSocketName;

	//maybe not reference directly ?
	TWeakObjectPtr<class UARItemWeapon> WeaponToModify;


public:	
	UPROPERTY()
		class APawn* POwner;
	// Sets default values for this component's properties
	UARWeaponManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(TSoftClassPtr<UGAAbilityBase> WeaponAbility);
	UGAAbilityBase* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon Manager")
		void AddToWeaponInventory(TSubclassOf<class UARItemWeapon> InWeapon);

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Add Weapon To Manager"), Category = "Weapon Manager")
		void BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);

	virtual void AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx);
	virtual void ServerAddWeaponToManager_Implementation(EAMGroup Group, EAMSlot Slot, int32 Idx);
	bool ServerAddWeaponToManager_Validate(EAMGroup Group, EAMSlot Slot, int32 Idx);

	UFUNCTION()
		void OnWeaponInputRead(TSoftClassPtr<UGAAbilityBase> WeaponAbilityTag, TArray<FGameplayTag> InInputTags);
protected:
	virtual void OnAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, const TArray<FGameplayTag>& InAbilityInput,
		EAMGroup InGroup, EAMSlot InSlot) override;
};
