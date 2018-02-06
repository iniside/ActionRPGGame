// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTags.h"
#include "AMTypes.h"
#include "ARWeaponBase.generated.h"

UCLASS()
class ACTIONRPGGAME_API AARWeaponBase : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		FGameplayTag WeaponAbility;
	UPROPERTY(EditAnywhere)
		FName SocketName;
	UPROPERTY()
		APawn* POwner;
	UPROPERTY()
		UARWeaponManagerComponent* WeaponManagerComponent;
public:	
	// Sets default values for this actor's properties
	AARWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnAddToWeaponManager(EAMGroup Group, EAMSlot Slot
		, class APawn* InPOwner
		, class UAMAbilityManagerComponent* WeaponManager);
	virtual void Equip();
	virtual void UnEquip();
	virtual void Holster(const FName& Socket);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Equip();
	void Multicast_Equip_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Holster(const FName& Socket);
	void Multicast_Holster_Implementation(const FName& Socket);

	UFUNCTION()
	virtual void OnWeaponAbilityReady(EAMGroup Group);
	
	inline void SetPawn(APawn* InPawn)
	{
		POwner = InPawn;
	}
	const inline FGameplayTag& GetWeaponAbility() const
	{
		return WeaponAbility;
	}

	virtual void OnRep_AttachmentReplication() override;
};
