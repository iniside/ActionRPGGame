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
		TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditAnywhere, Category = "Attachment Test")
		FName SocketName;

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

	TMap<EAMGroup, USkeletalMeshComponent*> GroupToComponent;
	
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

	
	void Equip(EAMGroup Group, TSoftObjectPtr<USkeletalMesh> InWeapon);
	void Holster(EAMGroup Group, TSoftObjectPtr<USkeletalMesh> InWeapon);
	inline void SetPOwner(APawn* InPawn)
	{
		POwner = InPawn;
	}

protected:
	void SetWeapon(TSoftObjectPtr<USkeletalMesh> InWeapon, USkeletalMeshComponent* Component);
	UFUNCTION()
		void OnRep_Group001HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group002HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group003HolsteredAttachment();
	UFUNCTION()
		void OnRep_Group004HolsteredAttachment();

	UFUNCTION()
		void OnRep_MainHandWeapon();
};
