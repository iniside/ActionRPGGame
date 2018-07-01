// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "IFInventoryComponent.h"
#include "IFEquipmentComponent.h"
#include "Weapons/ARWeaponAbilityBase.h"
#include "ARWeaponInventoryComponent.generated.h"

USTRUCT()
struct FARWeapon
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		TSoftClassPtr<class AARWeaponBase> Weapon;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		TSubclassOf<class UARItemWeapon> Item;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FVector Position;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FRotator Rotation;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		int8 NetIndex;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		int8 RepCounter;
};

UENUM()
enum class EARWeaponPosition
{
	Right= 0,
	Left = 1,
	BottomBack = 2,
	Side = 3,
	Equiped = 4
};

USTRUCT()
struct FARWeaponRPC
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FSoftClassPath Weapon;

	UPROPERTY()
		uint8 Index;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FVector Position;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FRotator Rotation;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		EARWeaponPosition AttachSlot;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAROnUpgradeInstalled, class UARItemWeapon*, Weapon, class UARWeaponUpgradeItem*, Upgrade, int8, WeaponIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAROnUpgradeRemoved, class UARItemWeapon*, Weapon, class UARWeaponUpgradeItem*, Upgrade, int8, WeaponIndex);
/*
	Manages currently equipped weapons (holstered and unholstered).
*/
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARWeaponInventoryComponent : public UIFEquipmentComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY()
		class APawn* POwner;

	UPROPERTY(EditAnywhere, CAtegory = "Input")
		TArray<FGameplayTag> WeaponInput;

	TMap<int8, UChildActorComponent*> GroupToComponent;

	TArray<FAFAbilitySpecHandle> ClientWeaponAbilities;
	TArray<FAFAbilitySpecHandle> ServerWeaponAbilities;

	int8 CurrentWeaponIndex;

	FAROnUpgradeInstalled OnUpgradeInstalled;
	FAROnUpgradeRemoved OnUpgradeRemoved;

public:	
	// Sets default values for this component's properties
	UARWeaponInventoryComponent();
	void BindInputs(UInputComponent* InputComponent, class UAFAbilityComponent* AbilityComponent);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	void InitializeWeapons(APawn* Pawn);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	inline FAROnUpgradeInstalled& GetOnUpgradeInstalled()
	{
		return OnUpgradeInstalled;
	}
	inline FAROnUpgradeRemoved& GetOnUpgradeRemoved()
	{
		return OnUpgradeRemoved;
	}
	virtual void OnClientPreItemAdded(UIFItemBase* Item, uint8 Index) override;
	virtual void OnItemAdded(UIFItemBase* Item, uint8 LocalIndex) override;
	UFUNCTION()
		void OnAbilityAdded(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle);
	virtual void OnItemRemoved(uint8 LocalIndex) override;

	virtual void OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex) override;
	virtual void OnServerItemRemoved(uint8 LocalIndex) override;

	void OnWeaponReady(TSoftClassPtr<UARWeaponAbilityBase> InAbilityTag, int8 Index);
	void SetAbilityToItem(int8 InLocalIndex, class UGAAbilityBase* InAbility);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastAddWeapon(const FARWeaponRPC& WeaponData);
	void MulticastAddWeapon_Implementation(const FARWeaponRPC& WeaponData);
	
	UFUNCTION(NetMulticast, Reliable)
		void MulticastRemoveWeapon(const FARWeaponRPC& WeaponData);
	void MulticastRemoveWeapon_Implementation(const FARWeaponRPC& WeaponData);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastEquipWeapon(int8 WeaponIndex, const FARWeaponRPC& WeaponData);
	void MulticastEquipWeapon_Implementation(int8 WeaponIndex, const FARWeaponRPC& WeaponData);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastUnequipWeapon(int8 OldWeaponIndex, const FARWeaponRPC& WeaponData);
	void MulticastUnequipWeapon_Implementation(int8 OldWeaponIndex, const FARWeaponRPC& WeaponData);

	void Equip(int8 WeaponIndex, const FARWeaponRPC& WeaponData);
	void Unequip(int8 WeaponIndex);
	void Holster();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHolster(const FARWeaponRPC& WeaponData);
	void ServerHolster_Implementation(const FARWeaponRPC& WeaponData);
	bool ServerHolster_Validate(const FARWeaponRPC& WeaponData);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastHolster(const FARWeaponRPC& WeaponData);
	void MulticastHolster_Implementation(const FARWeaponRPC& WeaponData);

	inline void SetPOwner(APawn* InPawn)
	{
		POwner = InPawn;
	}
	
	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	UFUNCTION(BlueprintCallable)
		void HolsterWeapon();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNextWeapon(int8 WeaponIndex);
	void ServerNextWeapon_Implementation(int8 WeaponIndex);
	bool ServerNextWeapon_Validate(int8 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientNextWeapon(int8 WeaponIndex, bool bPredictionSuccess);
	void ClientNextWeapon_Implementation(int8 WeaponIndex, bool bPredictionSuccess);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPreviousWeapon(int8 WeaponIndex);
	void ServerPreviousWeapon_Implementation(int8 WeaponIndex);
	bool ServerPreviousWeapon_Validate(int8 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientPreviousWeapon(int8 WeaponIndex, bool bPredictionSuccess);
	void ClientPreviousWeapon_Implementation(int8 WeaponIndex, bool bPredictionSuccess);

	void HandleClientPrediction(int8 WeaponIndex, bool bPredictionSuccess);

	UARItemWeapon* FindNextValid();
	UARItemWeapon* FindPreviousValid();

protected:
	void SetWeapon(const FARWeaponRPC& InWeapon, UChildActorComponent* Component);

	UFUNCTION()
		void AsynWeaponLoaded(UChildActorComponent* Component, FARWeaponRPC InWeapon);

public:
	//Local Indexes
	void AddMagazineMod(int8 WeaponIdx, int8 MagazineModIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddMagazineMod(int8 WeaponIdx, int8 MagazineModIndex);

	void ServerAddMagazineMod_Implementation(int8 WeaponIdx, int8 MagazineModIndex);
	bool ServerAddMagazineMod_Validate(int8 WeaponIdx, int8 MagazineModIndex);

	UFUNCTION(Client, Reliable)
		void ClientAddMagazineMod(int8 WeaponIdx, int8 MagazineModIndex);
	void ClientAddMagazineMod_Implementation(int8 WeaponIdx, int8 MagazineModIndex);

	void RemoveMagazineMod(int8 WeaponIdx);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRemoveMagazineMod(int8 WeaponIdx);

	void ServerRemoveMagazineMod_Implementation(int8 WeaponIdx);
	bool ServerRemoveMagazineMod_Validate(int8 WeaponIdx);

	UFUNCTION(Client, Reliable)
		void ClientRemoveMagazineMod(int8 WeaponIdx);
	void ClientRemoveMagazineMod_Implementation(int8 WeaponIdx);

};
