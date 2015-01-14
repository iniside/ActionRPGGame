// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GSGlobalTypes.h"
#include "../GSWeaponEquipmentTypes.h"
#include "GISInventoryBaseComponent.h"
#include "GSActiveActionsComponent.generated.h"


UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSActiveActionsComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FName  LeftHandName;

	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FGSWeaponSocketInfo  LeftSocketInfo;

	/*
		Multipliers for animations.
	 */
	float EquipingSpeedMultiplier;
	float UnequipingSpeedMultiplier;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		float WeaponSwapSpeed;

public:
	UPROPERTY()
		class UGSWeaponEquipmentComponent* LeftWeaponEquipment;
	UPROPERTY()
		class UGSWeaponEquipmentComponent* RightWeaponEquipment;
public:
	virtual void InitializeComponent() override;

	//refactoring into more meaningful names.
	void GetNextWeapon();
	void OnWeaponActive();



	void CopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerCopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

protected:
	UFUNCTION()
		void OnLeftWeaponRemoved(const FGISSlotSwapInfo& SlotSwapInfoIn);
	UFUNCTION()
		void OnRightWeaponRemoved(const FGISSlotSwapInfo& SlotSwapInfoIn);

	UFUNCTION()
		void OnWeaponEquipingEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
		int8 EquipWeapon;
	UFUNCTION()
		void OnRep_EquipWeapon();
	UPROPERTY(ReplicatedUsing = OnRep_UnequipWeapon)
		int8 UnequipWeapon;
	UFUNCTION()
		void OnRep_UnequipWeapon();

	UFUNCTION()
		void OnLeftHandReachedWeapon();

	UFUNCTION()
		void OnLeftHandReachedSheathe();

	UFUNCTION()
		void OnRightHandReachedWeapon();

	UFUNCTION()
		void OnRightHandReachedSheathe();


	void SwapWeapon(int32& LastCopiedIndex, class UGSItemWeaponInfo*& CurrentWeapon, class UGSItemWeaponInfo*& LastWeapon, class UGISInventoryBaseComponent* OtherInventory,
		int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex);
	/*
		Null pointer to weapons if nessessary, (for example weapon is going to be removed from socket).
		To prevent using that weapon, if another incompatibile weapon is equiped.
	 */
	void FinishSwappingWeapons(class UGSItemWeaponInfo* CurrentWeapon, class UGSItemWeaponInfo*& CurrentOppositeWeapon, class UGSItemWeaponInfo*& LastOppositeWeapon, int32 TabIndexIn);

	float CalculateAttachTimeLenght();

	void UpdateWeapons();

	void CheckLastWeapon(class UGSItemWeaponInfo* LastWeaponIn);
	void CheckCurrentWeapon(class UGSItemWeaponInfo* CurrentWeaponIn);
	/*
		If weapon is one handed checks if there are other weapon in hands
		which prevvent this weapon from equiping, and if needed unequip them(detach from socket).
	*/
	void CheckOneHand(class UGSItemWeaponInfo* LastWeapon, class UGSItemWeaponInfo* LastOppositeWeapon, class UGSItemWeaponInfo* CurrentOppositeWeapon);
	/*
		If weapon is two handed checks if there are other weapon in hands,
		which prevvent this weapon from equiping, and if needed unequip them(detach from socket).
	*/
	void CheckTwoHands(class UGSItemWeaponInfo* CurrentWeapon, class UGSItemWeaponInfo* LastLeftWeapon, class UGSItemWeaponInfo* LastRightWeapon);
	
	void UpdateSocket(class UGSItemWeaponInfo* WeaponIn);
protected:
	FTimerHandle TimerEquipStartHandle;
	FTimerHandle TimerUnquipStartHandle;

	FTimerHandle TimerRightEquipStartHandle;
	FTimerHandle TimerRightUnquipStartHandle;
private:
	int32 LastLeftCopiedIndex;
	int32 LastRightCopiedIndex;
	class IIGSEquipment* EquipInt;

	UPROPERTY()
	class UGSItemWeaponInfo* CurrentLeftHandWeapon;
	UPROPERTY()
	class UGSItemWeaponInfo* CurrentRightHandWeapon;

	UPROPERTY()
	class UGSItemWeaponInfo* LastLeftHandWeapon;
	UPROPERTY()
	class UGSItemWeaponInfo* LastRightHandWeapon;

	FName LastLeftSocket;
	FName LastRightSocket;

	void OnRightWeaponChanged();
	void OnLeftWeaponChanged();

	void UpdateCurrentLeftWeapon();
	void UpdateLastLeftWeapon();

	void UpdateCurrentRightWeapon();
	void UpdateLastRightWeapon();
}; 



