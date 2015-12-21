// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "IGISocket.h"
#include "IGISkeletalMesh.h"
#include "IGASAbilities.h"
#include "Items/IGSEquipment.h"

#include "GSCharacter.generated.h"

UCLASS(config=Game)
class GAMESYSTEM_API AGSCharacter : public ACharacter, public IIGSEquipment, public IIGISocket, public IIGISkeletalMesh, public IGameplayTagAssetInterface, public IIGASAbilities
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
	class UGSAbilitiesComponent* Abilities;

	/*
		This component will store current active actions, similiar to Dark Souls
		1. One ability.
		2. Two Weapons (left and right hand)
		3. One item to active.
	 */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	//class UGSActiveActionsComponent* ActiveActions;

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

	/*
		Tags desribing this character.
		Like organic, mechanic, crystal bla bla. You get the idea.

		Incoming damage, might want to know in advance type
		of target (which is determined by owned tags), to apply
		bonuses (or penalties) to damage in advance, before any other
		modifiers are going to be appiled.

		Technically I could also move it to attribute component
		but I'm not sure if would make much sense, beyond harder access to tags.

		Attribute component, is more about modifing any incoming and outgoing 
		attribute changes, based on their very specific type
		and owned attribute mods. 
		It does not care if attribute change is caused by shooting weapon,
		or using ability like spell. To accomodate for every specific weapon
		or ability, we would have to add attribute mod for every ability/weapon in game.

		Accessing type of object in advace should allows us to apply certain mods directly 
		on object which caused attribute change.

		Like special ammo type which might Cause 10% more damage to organic pawns,
		but causes 15% less to any mechanical targets.

		If organic target have have armor (like AttributeModArmor), it can be further modified,
		now on attribute component.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OwnedTags;

	UPROPERTY(EditAnywhere, Category = "Attach Socket")
		FName WeaponAttachSocket;
	UPROPERTY(EditAnywhere, Category = "Attach Socket")
		FName LeaftBackAttachmentSocket;
public:
	AGSCharacter(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void SwapSkeletalMesh(USkeletalMesh* SkeletalMesh);

	UFUNCTION(BlueprintCallable, Category = "Aim Offset")
	FRotator GetAimOffset() const;

	/** IIGSEquipment interface */
	virtual void SetSkeletalMesh(USkeletalMesh* MeshIn, FName ComponentNameIn) override;
	virtual void RemoveSkeletalMesh(FName ComponentNameIn) override;
	virtual void AttachActor(AActor* ActorIn, FName SocketNameIn = NAME_None) override;
	virtual USkeletalMeshComponent* GetSkeletalMeshComponentByName(FName NameIn) override;
	/* IIGSEquipment interface **/

	/** IIGISocket */
	//virtual FVector GetSocketLocation(FName SocketNameIn) override;
	/* IIGISocket **/

	/** IIGISkeletalMesh */
	virtual USkeletalMeshComponent* GetMasterSkeletalMesh() override;
	virtual FVector GetSocketLocation(FName SocketNameIn) override;
	/* IIGISkeletalMesh **/

	/** IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	/* IGameplayTagAssetInterface **/

	/** IIGASAbilities */
	virtual class UGASAbilitiesComponent* GetAbilityComp() override;
	/* IIGASAbilities **/
	
	FVector GetStartLocationForCrosshair();
	float GetCurrentWeaponSpread();

	void SetOnLeftCurrentWeaponChanged(class UGSItemWeaponInfo* WeaponIn);
protected:
	UPROPERTY()
		USkeletalMesh* DefaultLegMesh;
};

