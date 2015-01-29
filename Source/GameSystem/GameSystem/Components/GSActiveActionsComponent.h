// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GSGlobalTypes.h"
#include "../GSWeaponEquipmentTypes.h"
#include "GISInventoryBaseComponent.h"
#include "GSActiveActionsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGSOnLeftWeaponChanged, class UGSItemWeaponInfo*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGSOnLeftWeaponEquiped, class UGSItemWeaponInfo*, LeftWeaponIn);

USTRUCT()
struct GAMESYSTEM_API FGSCombatModeInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int8 RepCounter;
	UPROPERTY()
		bool bIsInCombat;
};
/*
	TODO:
	1. Ability equiping (that should be far less complicated)
	2. consumable item equiping (also should be pretty easy and not complicated).
	3. If weapon is two handed bind it to left mouse button.
	4. Only one ranged weapon can be equipped at time (for most of the time shouldn't be big deal
	since most ranged weapon are two handed..). 
	Not sure about it, but it's the only sane option i can think of, if i want to retain
	how I trace shooting. Especially if we mix abilities with weapon.
	Abilities are targeted using current weaponPosition.
	5. Can have two mele weapons, or mele + ranged weapon.


	New Design goal:
	1. One ranged weapon at time.
	2. Ranged weapon is always equiped to left hand (or rather input is in left hand).
	3. Mele weapons can be dual wielded. Normal binding rules apply (left to left).
	4. Can have mixed ranged and mele. Ranged left hand, mele right. With respective input.


	1. IF ability is begin casted, you can't use weapons.
	2. If you use weapon cancel casting ability (?).
	3. When you start using ability current weapon action is interrupted.
*/
UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSActiveActionsComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()

	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FName  LeftHandName;

	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FGSWeaponSocketInfo  LeftSocketInfo;
	/*
		Index of equiping section of montage.
	*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		int32 EquipingSectionIndex;

	/*
		Multipliers for animations.
	 */
	float EquipingSpeedMultiplier;
	float UnequipingSpeedMultiplier;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		float WeaponSwapSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float TimeInCombat;
	UPROPERTY(EditAnywhere, Category = "Action Widgets")
		TSubclassOf<class UGSWeaponInfoWidget> LeftWeaponInfoWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "Action Widgets")
		UGSWeaponInfoWidget* LeftWeaponInfoWidget;

	UPROPERTY(EditAnywhere, Category = "Action Widgets")
		TSubclassOf<class UGSWeaponInfoWidget> RightWeaponInfoWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "Action Widgets")
		UGSWeaponInfoWidget* RightWeaponInfoWidget;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FGSOnLeftWeaponEquiped OnLeftWeaponEquipedDel;

public:
	UPROPERTY()
		class UGSWeaponEquipmentComponent* LeftWeaponEquipment;
	UPROPERTY()
		class UGSWeaponEquipmentComponent* RightWeaponEquipment;
public:
	virtual void InitializeComponent() override;
	////////////////////
	////////// Weapons
	//refactoring into more meaningful names.
	void GetNextWeapon();
	void OnWeaponActive();



	void SetWeaponFrom(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetWeaponFrom(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

	/*
		Probabaly should do it on server.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputPressLeftWeapon();
	void InputPressLeftWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputReleaseLeftWeapon();
	void InputReleaseLeftWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPressRightWeapon();
	void InputPressRightWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReleaseRightWeapon();
	void InputReleaseRightWeapon();

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
	void FinishSwappingWeapons(class UGSItemWeaponInfo*& CurrentWeapon, class UGSItemWeaponInfo*& CurrentOppositeWeapon,
	class UGSItemWeaponInfo*& LastOppositeWeapon, int32 OppositeTabIndexInm, int32 CurrentTabIndexIn);

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

	FTimerHandle TimerLeaveCombat;
private:
	int32 LastLeftCopiedIndex;
	int32 LastRightCopiedIndex;
	class IIGSEquipment* EquipInt;
public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentLeftHandWeapon)
	class UGSItemWeaponInfo* CurrentLeftHandWeapon;
	UFUNCTION()
		void OnRep_CurrentLeftHandWeapon();
	UPROPERTY(ReplicatedUsing = OnRep_CurrentRightHandWeapon)
	class UGSItemWeaponInfo* CurrentRightHandWeapon;
	UFUNCTION()
		void OnRep_CurrentRightHandWeapon();

	UPROPERTY()
		class AGSWeaponRanged* LeftRangedWeapon;
	UPROPERTY()
		class AGSWeaponRanged* RightRangedWeapon;

	FGSOnLeftWeaponChanged OnLeftWeaponChangedEvent;

	UPROPERTY(BlueprintReadOnly, Replicated)
		EGSWeaponEquipState WeaponEquipState;

	UPROPERTY(BlueprintReadOnly, Replicated)
		bool bIsInCombat;

	void OnLeaveCombatTimer();

	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetLeftHandIdleAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetLeftHandCombatAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetLeftHandMoveAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetRightHandIdleAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetRightHandCombatAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetRightHandMoveAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetBothHandsIdleAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetBothHandsCombatAnim() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAnimSequence* GetBothHandsMoveAnim() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
		const float GetCurrentWeaponHorizontalRecoil() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
		const float GetCurrentWeaponVerticalRecoil() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Animations")
		UAimOffsetBlendSpace* GetBothHandWeaponAimBlend() const;
private:
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
public:
	void InputReloadWeapon(int32 TabIndex, int32 SlotIndex);

//////////////////////////////////////////
////////////// Ability Handling
private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentAbility)
	class UGSAbilityInfo* CurrentAbility;
	UFUNCTION()
	void OnRep_CurrentAbility();
public:

	void SetAbility(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 OtherSlotIndex);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAbility(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 OtherSlotIndex);
	
	void InputAbilityPressed();
	void InputAbilityReleased();
}; 



