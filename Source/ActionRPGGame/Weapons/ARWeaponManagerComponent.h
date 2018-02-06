// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "ARWeaponsTypes.h"
#include "AMAbilityManagerComponent.h"
#include "ARWeaponManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FAROnWeaponReady, class UARWeaponAbilityBase*);


USTRUCT()
struct FARWeaponItem : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
		class AARWeaponBase* Weapon;
	UPROPERTY()
		EAMGroup Group;

	FARWeaponItem()
		: Weapon(nullptr)
		, Group(EAMGroup::MAX)
	{}

	FARWeaponItem(AARWeaponBase* InWeapon, EAMGroup InGroup)
		: Weapon(InWeapon)
		, Group(InGroup)
	{}

public:
	void PreReplicatedRemove(const struct FARWeaponContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FARWeaponContainer& InArraySerializer);
	void PostReplicatedChange(const struct FARWeaponContainer& InArraySerializer);

	bool operator==(const FARWeaponItem& Other) const
	{
		return Weapon == Other.Weapon;
	}
};

USTRUCT()
struct FARWeaponContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	
	friend struct FARWeaponItem;
public:
	UPROPERTY()
		TArray<FARWeaponItem> Weapons;
protected:
	UPROPERTY()
		class UARWeaponManagerComponent* WeaponManager;

public:
	void Initialize(class UARWeaponManagerComponent* InWeaponManager);
	void AddWeapon(const FARWeaponItem& InWeapon);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FARWeaponItem, FARWeaponContainer>(Weapons, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FARWeaponContainer > : public TStructOpsTypeTraitsBase2<FARWeaponContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
		TArray<TSubclassOf<class AARWeaponBase>> WeaponClasses;

	UPROPERTY(EditAnywhere, Category = "Attachment Config")
		TArray<FARWeaponAttachment> WeaponAttachment;


	//currently equipped weapons
	UPROPERTY()
		TArray<class AARWeaponBase*> Weapons;

	//currently active weapons.
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Manager")
		class AARWeaponBase* CurrentWeapon;

	TMap<FGameplayTag, AARWeaponBase*> AbilityToWeapon;

	UPROPERTY(Replicated)
		FARWeaponContainer EquippedWeapons;

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

	void OnWeaponAbilityReady(const FGameplayTag& WeaponAbility, AARWeaponBase* InWeapon, EAMGroup InGroup);

	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
protected:
	virtual void OnAbilityReady(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InAbilityInput,
		EAMGroup InGroup, EAMSlot InSlot) override;

	virtual void OnNextGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;
	virtual void OnPreviousGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;

	void EquipWeapon(const FGameplayTag& PreviousWeaponTag, const FGameplayTag& NextWeaponTag);

	FGameplayTag FindNextValid();
	FGameplayTag FindPreviousValid();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_HolsterWeapon(AARWeaponBase* InWeapon, EAMGroup InGroup);
	void Server_HolsterWeapon_Implementation(AARWeaponBase* InWeapon, EAMGroup InGroup);
	bool Server_HolsterWeapon_Validate(AARWeaponBase* InWeapon, EAMGroup InGroup);

};
