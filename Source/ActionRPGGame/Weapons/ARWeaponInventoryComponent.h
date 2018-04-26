// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "IFInventoryComponent.h"
#include "IFEquipmentComponent.h"
#include "AMTypes.h"

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
		uint8 NetIndex;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		uint8 RepCounter;
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
		TSoftClassPtr<class AARWeaponBase> Weapon;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FVector Position;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FRotator Rotation;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		EARWeaponPosition AttachSlot;
};


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

	TMap<uint8, UChildActorComponent*> GroupToComponent;
	TMap<EAMGroup, UARItemWeapon*> GroupToItem;

	uint8 CurrentWeaponIndex;

public:	
	// Sets default values for this component's properties
	UARWeaponInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void InitializeWeapons(APawn* Pawn);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnItemAdded(UIFItemBase* Item, uint8 LocalIndex) override;
	virtual void OnItemRemoved(uint8 LocalIndex) override;

	virtual void OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex) override;
	virtual void OnServerItemRemoved(uint8 LocalIndex) override;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastAddWeapon(const FARWeaponRPC& WeaponData);
	void MulticastAddWeapon_Implementation(const FARWeaponRPC& WeaponData);
	
	UFUNCTION(NetMulticast, Reliable)
		void MulticastRemoveWeapon(const FARWeaponRPC& WeaponData);
	void MulticastRemoveWeapon_Implementation(const FARWeaponRPC& WeaponData);


	void Equip(uint8 WeaponIndex, class UARItemWeapon* InWeapon);
	void Unequip(uint8 WeaponIndex);
	void Holster(EAMGroup Group, class UARItemWeapon* InWeapon);
	inline void SetPOwner(APawn* InPawn)
	{
		POwner = InPawn;
	}
	void SetAbilityToItem(uint8 InLocalIndex, class UGAAbilityBase* InAbility);
	UFUNCTION(BlueprintCallable)
		void NextWeapon();
	UFUNCTION(BlueprintCallable)
		void PreviousWeapon();

	UFUNCTION(BlueprintCallable)
		void HolsterWeapon();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNextWeapon(uint8 WeaponIndex);
	void ServerNextWeapon_Implementation(uint8 WeaponIndex);
	bool ServerNextWeapon_Validate(uint8 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientNextWeapon(uint8 WeaponIndex, bool bPredictionSuccess);
	void ClientNextWeapon_Implementation(uint8 WeaponIndex, bool bPredictionSuccess);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPreviousWeapon(uint8 WeaponIndex);
	void ServerPreviousWeapon_Implementation(uint8 WeaponIndex);
	bool ServerPreviousWeapon_Validate(uint8 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientPreviousWeapon(uint8 WeaponIndex, bool bPredictionSuccess);
	void ClientPreviousWeapon_Implementation(uint8 WeaponIndex, bool bPredictionSuccess);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHolsterWeapon(uint8 WeaponIndex);
	void ServerHolsterWeapon_Implementation(uint8 WeaponIndex);
	bool ServerHolsterWeapon_Validate(uint8 WeaponIndex);

	UARItemWeapon* FindNextValid();
	UARItemWeapon* FindPreviousValid();

protected:
	void SetWeapon(const FARWeaponRPC& InWeapon, UChildActorComponent* Component);

	UFUNCTION()
		void AsynWeaponLoaded(UChildActorComponent* Component, FARWeaponRPC InWeapon);

public:
	//Local Indexes
	void AddMagazineMod(uint8 WeaponIdx, uint8 MagazineModIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddMagazineMod(uint8 WeaponIdx, uint8 MagazineModIndex);

	void ServerAddMagazineMod_Implementation(uint8 WeaponIdx, uint8 MagazineModIndex);
	bool ServerAddMagazineMod_Validate(uint8 WeaponIdx, uint8 MagazineModIndex);

};
