// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISInventoryBaseComponent.h"
#include "GSAbilitiesComponent.generated.h"


DECLARE_DELEGATE(FGSOnAbilityCastStarted);

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSAbilitiesComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
public:
	virtual void InitializeComponent() override;

	UPROPERTY()
		TArray<class AGSAbility*> Abilities;

	UPROPERTY()
		class AGSAbility* ActiveAbility;
	
	UPROPERTY(EditAnywhere, Category = "Ability Config")
		TSubclassOf<class UGSAbilityCastTimeWidget> CastTimeWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD Widgets")
		class UGSAbilityCastTimeWidget* CastTimeWidget;
	/*
		Indicates that ability casting have started
		and it will tell widget to display current cast time.
	*/
	FGSOnAbilityCastStarted OnAbilityCastStarted;

	/** UGISInventoryBaseComponent - overrides */
	/*
	*/
	virtual void InputSlotPressed(int32 TabIndex, int32 SlotIndex) override;
	virtual void InputSlotReleased(int32 TabIndex, int32 SlotIndex) override;
	/** UGISInventoryBaseComponent - overrides */

	/*
		Add:
		1. Cooldown display
		2. Checking if amy active ability is currently being casted.
		3. Display current cast time.
	*/
};



