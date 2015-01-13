// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "Items/IGSEquipment.h"

#include "GSCharacter.generated.h"

UCLASS(config=Game)
class GAMESYSTEM_API AGSCharacter : public ACharacter, public IIGSEquipment
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UGSEquipmentComponent* Equipment;

	/*
		Component used to store currently active weapons.
		Delibertly different from EquipmentComponent, as it have set of different requriments.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UGSWeaponEquipmentComponent* WeaponsEquipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UGSWeaponEquipmentComponent* RightWeaponsEquipment;

	/*
		This component will store current active actions, similiar to Dark Souls
		1. One ability.
		2. Two Weapons (left and right hand)
		3. One item to active.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UGSActiveActionsComponent* ActiveActions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class USkeletalMeshComponent* HeadComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class USkeletalMeshComponent* ChestComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class USkeletalMeshComponent* HandsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class USkeletalMeshComponent* LegsComp;

	static const FName HeadSlotComponent;
	static const FName ChestSlotComponent;
	static const FName HandsSlotComponent;
	static const FName LegsSlotComponent;

	UPROPERTY(EditAnywhere, Category = "Attach Socket")
		FName WeaponAttachSocket;
	UPROPERTY(EditAnywhere, Category = "Attach Socket")
		FName LeaftBackAttachmentSocket;
public:
	AGSCharacter(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void SwapSkeletalMesh(USkeletalMesh* SkeletalMesh);

	/** IIGSEquipment interface */
	virtual void SetSkeletalMesh(USkeletalMesh* MeshIn, FName ComponentNameIn) override;
	virtual void RemoveSkeletalMesh(FName ComponentNameIn) override;
	virtual void AttachActor(AActor* ActorIn, FName SocketNameIn = NAME_None) override;
	virtual USkeletalMeshComponent* GetSkeletalMeshComponentByName(FName NameIn) override;
	/* IIGSEquipment interface **/

protected:
	UPROPERTY()
		USkeletalMesh* DefaultLegMesh;
};

