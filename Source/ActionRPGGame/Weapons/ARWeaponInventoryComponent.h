// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "IFInventoryComponent.h"
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

/*
	Manages currently equipped weapons (holstered and unholstered).
*/
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARWeaponInventoryComponent : public UIFInventoryComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Group001HolsteredAttachment)
		FARWeapon Group001HolsteredAttachment;
	UPROPERTY(ReplicatedUsing = OnRep_Group002HolsteredAttachment)
		FARWeapon Group002HolsteredAttachment;
	UPROPERTY(ReplicatedUsing = OnRep_Group003HolsteredAttachment)
		FARWeapon Group003HolsteredAttachment;
	UPROPERTY(ReplicatedUsing = OnRep_Group004HolsteredAttachment)
		FARWeapon Group004HolsteredAttachment;
	UPROPERTY(ReplicatedUsing = OnRep_MainHandWeapon)
		FARWeapon MainHandWeapon;

	UPROPERTY()
		class APawn* POwner;

	TMap<uint8, UChildActorComponent*> GroupToComponent;
	TMap<EAMGroup, UARItemWeapon*> GroupToItem;

	TArray<FARWeapon*> WeaponHelper;

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
	void SetWeapon(const FARWeapon& InWeapon, UChildActorComponent* Component);
	UFUNCTION()
		void OnRep_Group001HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group002HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group003HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group004HolsteredAttachment();

	UFUNCTION()
		void OnRep_MainHandWeapon(FARWeapon OldWeapon);

	UFUNCTION()
		void AsynWeaponLoaded(UChildActorComponent* Component, FARWeapon InWeapon);
};
