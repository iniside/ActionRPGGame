// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilitiesComponent.h"
#include "GSAbilitiesComponent.generated.h"

/*
	Struct representing available abilities to this component.
	It does not represent abilities which are Instanced (ActiveAbilities array).
*/
USTRUCT()
struct GAMESYSTEM_API FGSAvailableAbilities
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TSubclassOf<class UGSAbility> AbilityClass;
	/*
		If this ability is active, store index of array, in which instanced ability is contained.
	*/
	UPROPERTY()
		int32 ActiveAbilityIndex;

	UPROPERTY()
		bool bIsAbilityActive;
};


USTRUCT()
struct GAMESYSTEM_API FGSAbilitySlot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int32 AbilityIndex;

	UPROPERTY()
		int32 SlotIndex;

	UPROPERTY()
		int32 SetIndex;

	FGSAbilitySlot()
	{
		AbilityIndex = INDEX_NONE;
		SlotIndex = INDEX_NONE;
		SetIndex = INDEX_NONE;
	}
};
USTRUCT()
struct GAMESYSTEM_API FGSAbilitiesSets
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int32 SetIndex;

	TArray<FGSAbilitySlot> AbilitySlots;
	FGSAbilitiesSets()
	{
		SetIndex = INDEX_NONE;
	}
};
USTRUCT()
struct GAMESYSTEM_API FGSActiveAbilitiesSlotConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 NumberOfSlots;
};

DECLARE_DELEGATE(FGSOnAbilityAddedToSet);
DECLARE_DELEGATE_OneParam(FGSOnGetAbilityIndex, int32);

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSAbilitiesComponent : public UGASAbilitiesComponent
{
	GENERATED_BODY()
public:
	UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(ReplicatedUsing = OnRep_OwnedAbilities)
		TArray<FGSAvailableAbilities> OwnedAbilities;
	UFUNCTION()
		void OnRep_OwnedAbilities();
	/*
		Contains list of abilities which can be activated trough
		input binding or UI.

	*/
	UPROPERTY(Replicated)
		TArray<FGSAbilitiesSets> AbilitySets;

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
	
	FGSOnAbilityAddedToSet OnAbilityAddedToSet;

	FGSOnGetAbilityIndex OnGetAbilityIndex;
protected:
	int32 CurrentAbility;
public:
	virtual void InitializeComponent() override;
	inline class UGSAbility* GetGSAbility(int32 IndexIn);
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
private:
	void InitializeActivatableAbilities();

	UFUNCTION()
		void Del_OnAbilityCasted();
};



