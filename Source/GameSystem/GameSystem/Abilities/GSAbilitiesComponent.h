// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilitiesComponent.h"
#include "GSAbilityGlobalTypes.h"
#include "GSAbilitiesComponent.generated.h"

DECLARE_DELEGATE(FGSOnAbilityAddedToSet);
DECLARE_DELEGATE_OneParam(FGSOnGetAbilityIndex, int32);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGSOnAbilityPressedIndex, int32, Index);
/* Blueprintable.. becaue why not ? Not that there are many extension points right now ;) */
UCLASS(Blueprintable, hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSAbilitiesComponent : public UGASAbilitiesComponent
{
	GENERATED_BODY()
public:
	/* Normally you should load it from somwhere. File, database sth. */
	UPROPERTY(ReplicatedUsing = OnRep_OwnedAbilities)
		FGSAbilityBook OwnedAbilities;
	UFUNCTION()
		void OnRep_OwnedAbilities();

	FGSOnAbilityAddedToSet OnAbilityAddedToBook;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Abilities")
		FGSOnAbilityPressedIndex OnAbilityPressedIndex;

	/*
		Contains list of abilities which can be activated trough
		input binding or UI.

	*/
	UPROPERTY(ReplicatedUsing=OnRep_AbilitySets)
		TArray<FGSAbilitiesSets> AbilitySets;
	UFUNCTION()
		void OnRep_AbilitySets();

	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		TSubclassOf<class UGSAbilityContainerWidget> AbilityContainerClass;

	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		TSubclassOf<class UGSAbilityTabWidget> AbilityTabClass;

	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		TSubclassOf<class UGSAbilitySlotWidget> AbilitySlotClass;

	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		TSubclassOf<class UGSAbilityWidget> AbilityWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		ESlateVisibility ActionBarVisibility;
	UPROPERTY(EditAnywhere, Category = "Action Bar UI")
		FName ActionBarAbilitySlotName;

	UPROPERTY(BlueprintReadOnly, Category = "Action Bar UI")
		UGSAbilityContainerWidget* AbilityContainer;


	UPROPERTY(EditAnywhere, Category = "Ability Book UI")
		FGSAbilityBookUI AbilityBookConfig;
	UPROPERTY(EditAnywhere, Category = "Ability Book UI")
		TArray<FGSAbilityBookConfig> AbilityBookSlotsConfig;

	UPROPERTY(BlueprintReadOnly, Category = "Ability Book UI")
	class UGSAbilityBookContainerWidget* AbilityBookContainer;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UGSAbilityCastTimeWidget> CastTimeWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		UGSAbilityCastTimeWidget* CastTimeWidget;
	/*
		One element in array is one Set.
		Think of set like hotbar, or something like that ;).

		NumberOfSlots is amount of buttons (slots) in your Set (Hotbar).
	*/
	UPROPERTY(EditAnywhere, Category = "UI Config")
		TArray<FGSActiveAbilitiesSlotConfig> ActivatableAbilitiesConfig;
	

	UPROPERTY(BlueprintReadOnly, Category = "Base")
	class AGSPlayerController* PCOwner;

	FGSOnAbilityAddedToSet OnAbilityAddedToSet;

	FGSOnGetAbilityIndex OnGetAbilityIndex;
protected:
	int32 CurrentAbility;
public:
	UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	/*
		Initializes guid widgets. Call it after there is valid player controller available.
	*/
	void InitializeGUI();

	class UGSAbility* GetGSAbility(int32 IndexIn);

	/*
		Automatically inserts ability into action bar.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability System")
		void GiveAbilityAndInsert(TSubclassOf<class  UGSAbility> AbilityIn, int32 SetIndex, int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerGiveAbilityAndInsert(TSubclassOf<class  UGSAbility> AbilityIn, int32 SetIndex, int32 SlotIndex);

	virtual void ServerGiveAbilityAndInsert_Implementation(TSubclassOf<class  UGSAbility> AbilityIn, int32 SetIndex, int32 SlotIndex);
	virtual bool ServerGiveAbilityAndInsert_Validate(TSubclassOf<class  UGSAbility> AbilityIn, int32 SetIndex, int32 SlotIndex );
	/*
		Gives Ability to this pawn, inserting into list of available abilities.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability System")
		virtual void GiveAbility(TSubclassOf<class  UGSAbility> AbilityIn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerGiveAbility(TSubclassOf<class  UGSAbility> AbilityIn);
	virtual void ServerGiveAbility_Implementation(TSubclassOf<class  UGSAbility> AbilityIn);
	virtual bool ServerGiveAbility_Validate(TSubclassOf<class  UGSAbility> AbilityIn);

	UFUNCTION(BlueprintPure, Category = "Ability System|UI")
		float GetCurrentCastTime();
	UFUNCTION(BlueprintPure, Category = "Ability System|UI")
		float GetCastTime();
	/*
		Another variant of Input for activating abilities.
	*/
	void InputPressed(int32 SetIndex, int32 SlotIndex);
	void InputReleased(int32 SetIndex, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Abilities Test")
	void BP_AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex, int32 AbilityIndex);

	void AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex, int32 AbilityIndex);
	void AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex,
		 int32 LastSetIndex, int32 LastSlotIndex, int32 AbilityIndex);

	void AddAbilityToSlotFromBook(int32 TargetSetIndex, int32 TargetSlotIndex, int32 BookTabIndex,
		int32 BookSlotIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddAbilityToSlotFromBook(int32 TargetSetIndex, int32 TargetSlotIndex, int32 BookTabIndex,
		int32 BookSlotIndex);
	virtual void ServerAddAbilityToSlotFromBook_Implementation(int32 TargetSetIndex, int32 TargetSlotIndex,
		int32 BookTabIndex, int32 BookSlotIndex);
	virtual bool ServerAddAbilityToSlotFromBook_Validate(int32 TargetSetIndex, int32 TargetSlotIndex,
		int32 BookTabIndex, int32 BookSlotIndex);

private:
	void InitializeActivatableAbilities();
	void InitializeAbilityBook();
	UFUNCTION()
		void Del_OnAbilityCasted();
};



