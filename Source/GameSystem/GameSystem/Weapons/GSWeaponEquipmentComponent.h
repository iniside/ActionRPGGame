// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GSWeaponEquipmentTypes.h"
#include "../GSGlobalTypes.h"

#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "GSWeaponEquipmentComponent.generated.h"

struct GAMESYSTEM_API FGSSlotNamesHelper
{
	static const FName BackLeftSlot;
	static const FName BackRightSlot;
	static const FName HipLeftSlot;
	static const FName HipRightSlot;
	static const FName ShieldSlot;
	static const FName HipBackSlot;
};

USTRUCT(Blueprintable)
struct GAMESYSTEM_API FGSWeaponSetSlot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName SlotName;

	UPROPERTY(EditAnywhere)
		int32 SlotIndex;
	UPROPERTY(EditAnywhere)
		int32 SetIndex;
	/*
		And index to array element containing referenced weapon.
	*/
	UPROPERTY()
		int32 WeaponID;
	UPROPERTY()
		int32 WeaponTabID;


	FGSWeaponSetSlot()
	{
		WeaponID = -1;
		WeaponTabID = -1;
	};
};

USTRUCT(Blueprintable)
struct GAMESYSTEM_API FGSWeaponSet
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		FName SetName;
	/*
		True if set is currently equiped (ie attached to hands, or just active, but sheathed to their
		normal attachment points other hands.
	*/
	UPROPERTY()
		bool bIsSetEquiped;

	UPROPERTY(EditAnywhere)
		TArray<FGSWeaponSetSlot> Weapons;

	FGSWeaponSet()
	{
		bIsSetEquiped = false;
	}
};
/*
	Now that we have weapon sets, we probabaly can make stance animations per weapon set, instead
	per weapon.
	1. Lots of weapon can share single stance ie. Rifle or shotgun 
	can have the same hip/ironsight stance.
	2. Pistol is unique case.
	3. Two Handed mele weapons also need different stance (this can be different for 
	weapons with long and short handle, like warhammer and two handed sword).
	4. Dual wield two handed weapon can use normal relaxed stance, but will need to use separate
	one for while in combat state.
*/
DECLARE_DELEGATE(FGSOnWeaponSetsInitialized);
DECLARE_DELEGATE(FGSOnWeaponSetUpdated);

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSWeaponEquipmentComponent : public UGISInventoryBaseComponent
{
	GENERATED_BODY()
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
public:
	UGSWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	UFUNCTION()
		void AttachActorTo(AActor* ActorIn, FName& AttachedSocketNameOut, const TArray<FGSWeaponSocketInfo>& PossibleSockets);

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponWidget> LeftWeaponWidgetClass;
	UPROPERTY(EditAnywhere, Category = "UI")
		ESlateVisibility LeftWeaponVisibility;
	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponWidget> RightWeaponWidgetClass;
	

	UPROPERTY(EditAnywhere, Category = "UI")
		ESlateVisibility WeaponSetsVisibility;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponSetContainerWidget> WeaponSetContainerClass;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponSetWidget> SetWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponSetSlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSWeaponSetItemWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		UGSWeaponSetContainerWidget* WeaponSetContainer;

	UPROPERTY(BlueprintReadOnly)
	class UGSWeaponWidget* LeftWeaponWidget;
	UPROPERTY(BlueprintReadOnly)
	class UGSWeaponWidget* RightWeaponWidget;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float BaseEquipTime;
	UPROPERTY(EditAnywhere, Category = "Settings")
		int32 EquipingSectionIndex;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FName WeaponSetDropSlot;
	/*
		Add new elements, to indicate how many possible weapons set can be created.
	*/
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_WeaponSets, Category = "Settings")
		TArray<FGSWeaponSet> WeaponSets;

	UPROPERTY(EditAnywhere, Category = "Settings")
		int32 MainHandSlotIndex;
	UPROPERTY(EditAnywhere, Category = "Settings")
		int32 OffHandSlotIndex;

	UFUNCTION()
	void OnRep_WeaponSets();
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FGSWeaponSocketInfo  OffHandSocketInfo;
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FGSWeaponSocketInfo  MainHandSocketInfo;

	FGSOnWeaponSetsInitialized OnWeaponSetsInitialized;
	FGSOnWeaponSetUpdated OnWeaponSetUpdated;

	UPROPERTY(BlueprintReadOnly, Replicated)
		EGSWeaponEquipState WeaponEquipState;

	bool bIsInCombatStance;
public:
	/**
	 *	Contains all sockets, to which we can attach weapon.
	 *	Also contains information if something is attached to this socket.
	 */
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		TArray<FGSEquipSocketInfo> AttachmentSockets;

	UPROPERTY()
		UGISInventoryBaseComponent* Inventory;

	UPROPERTY(ReplicatedUsing=OnRep_MainHandWeapon)
	class UGSItemWeaponInfo* MainHandWeapon;
	UFUNCTION()
		void OnRep_MainHandWeapon();
	
	UPROPERTY(ReplicatedUsing = OnRep_OffHandWeapon)
	class UGSItemWeaponInfo* OffHandWeapon;
	UFUNCTION()
		void OnRep_OffHandWeapon();
protected:
	UPROPERTY()
	class UGSItemWeaponInfo* LastLeftWeapon;
	UFUNCTION()
		void OnRep_LastLeftWeapon();
	UPROPERTY()
	class UGSItemWeaponInfo* LastRightWeapon;

	class IIGSEquipment* EquipInt;
	FName SocketToAttach;
protected:
	int32 LastWeaponSet;

	FTimerHandle TimerEquipStartHandle;
	FTimerHandle TimerUnquipStartHandle;

	FTimerHandle TimerRightEquipStartHandle;
	FTimerHandle TimerRightUnquipStartHandle;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponEquiped)
		int8 ReplicateWeaponEquiped;
	UFUNCTION()
		void OnRep_WeaponEquiped();
public:
	void AddWeaponToSet(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddWeaponToSet(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot);
	virtual void ServerAddWeaponToSet_Implementation(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot);
	virtual bool ServerAddWeaponToSet_Validate(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot);

	void AddWeaponToSet(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddWeaponToSetFourParams(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot);
	virtual void ServerAddWeaponToSetFourParams_Implementation(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot);
	virtual bool ServerAddWeaponToSetFourParams_Validate(int32 SlotIndex, int32 TabIndex,
		const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot);


	void CheckWeaponSet(int32 SetIndex);
	void AddWeaponToSlotInSet(int32 WeaponIDIn, int32 WeaponTabIDIn, int32 SlotIndex,
		int32 SetIndex);
	UFUNCTION()
		void OnItemFromInventory(const FGISSlotSwapInfo& SlotSwapInfoIn);

	UFUNCTION()
		void OnItemWeapEquip(const FGISSlotSwapInfo& SlotSwapInfoIn);

	/////////////////////////
	////////// Input Begin
	void InputLeftWeaponPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputLeftWeaponPressed();
	void ServerInputLeftWeaponPressed_Implementation();
	bool ServerInputLeftWeaponPressed_Validate();

	void InputLeftWeaponReleased();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputLeftWeaponReleased();
	void ServerInputLeftWeaponReleased_Implementation();
	bool ServerInputLeftWeaponReleased_Validate();

	void InputRightWeaponPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputRightWeaponPressed();
	void ServerInputRightWeaponPressed_Implementation();
	bool ServerInputRightWeaponPressed_Validate();

	void InputRightWeaponReleased();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputRightWeaponReleased();
	void ServerInputRightWeaponReleased_Implementation();
	bool ServerInputRightWeaponReleased_Validate();
	////////// Input End
	////////////////////////////

	virtual bool CheckIfCanAddItemToSlot(class UGISItemData* TargetDataIn, int32 TargetTabIndex, int32 TargetSlotIndex,
	class UGISItemData* LastDataIn) override;
	
	void SwapWeaponSet();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeaponSet();
	virtual void ServerSwapWeaponSet_Implementation();
	virtual bool ServerSwapWeaponSet_Validate();
protected:
	void SimulateSwapWeaponSet(); //that's not really good name since it run on server and owning client.
public:

	/*
		Which animation will be return will depend on current state of pawn.
		We don't want bazylion functions, each returning exactly one sequence,
		which then must be bind to one path in anim graph.

		We want single path in anim graph, which will operate only on one function.
	*/
	UFUNCTION(BlueprintPure, Category = "Weapon Animation")
		UAnimSequence* GetMainHandAnimSequence() const;
	UFUNCTION(BlueprintPure, Category = "Weapon Animation")
		UAnimSequence* GetOffHandAnimSequence() const;

	UAnimSequence* GetWeaponAnimSequence(class UGSItemWeaponInfo* WeaponIn) const;
protected:
	void UpdateWeapons();
	UAnimMontage* FindEquipMontage(class UGSItemWeaponInfo* WeaponIn);
	float CalculateAttachTimeLenght(class UGSItemWeaponInfo* WeaponIn);
	void UpdateMainHandWeapon();
	void UpdateLastLeftWeapon();
	void OnLeftHandReachedWeapon();
	void OnLeftHandReachedSheathe();
	void UnequipWeapon();
private:
	bool CheckIfStandloneOrAuthority();
};



