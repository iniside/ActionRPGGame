// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "AMTypes.h"

#include "ARWeaponPawnManagerComponent.generated.h"

USTRUCT()
struct FARWeapon
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		TSoftClassPtr<class AARWeaponBase> Weapon;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FVector Position;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FRotator Rotation;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		EAMGroup Group;

	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		uint8 RepCounter;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARWeaponPawnManagerComponent : public UActorComponent
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

	TMap<EAMGroup, UChildActorComponent*> GroupToComponent;
	
	TArray<FARWeapon*> WeaponHelper;

public:	
	// Sets default values for this component's properties
	UARWeaponPawnManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void Equip(EAMGroup Group, class UARItemWeapon* InWeapon);
	void EquipInactive(EAMGroup Group, UARItemWeapon* InWeapon, EAMGroup OldGroup, UARItemWeapon* InOldWeapon);
	void Holster(EAMGroup Group, class UARItemWeapon* InWeapon);
	void HolsterActive(EAMGroup Group);
	inline void SetPOwner(APawn* InPawn)
	{
		POwner = InPawn;
	}

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
