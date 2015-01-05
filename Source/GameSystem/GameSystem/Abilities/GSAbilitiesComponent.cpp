// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "GSAbility.h"
#include "GSAbilityInfo.h"

#include "Widgets/GSAbilityCastTimeWidget.h"

#include "GSAbilitiesComponent.h"

UGSAbilitiesComponent::UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UGSAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ENetRole CurrentRole = GetOwnerRole();
	ENetMode CurrentNetMode = GetNetMode();

	if (CurrentRole < ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
	{
		UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
		if (CastTimeWidgetClass)
		{
			CastTimeWidget = ConstructObject<UGSAbilityCastTimeWidget>(CastTimeWidgetClass, Outer);
			if (CastTimeWidget)
			{
				ULocalPlayer* Player = World->GetFirstLocalPlayerFromController(); //temporary
				CastTimeWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
				CastTimeWidget->Initialize();
				CastTimeWidget->AbilityComponent = this;
				CastTimeWidget->SetVisibility(InventoryVisibility);
			}
		}
	}
}

void UGSAbilitiesComponent::InputSlotPressed(int32 TabIndex, int32 SlotIndex)
{
	if (Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData)
	{
		UGSAbilityInfo* abilityPtr = Cast<UGSAbilityInfo>(Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData);
		if (abilityPtr)
		{
			ActiveAbility = abilityPtr->ActiveAbility;
			abilityPtr->InputPressed();
		}
	}
}
void UGSAbilitiesComponent::InputSlotReleased(int32 TabIndex, int32 SlotIndex)
{
	if (Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData)
	{
		UGSAbilityInfo* abilityPtr = Cast<UGSAbilityInfo>(Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData);
		if (abilityPtr)
		{
			//ActiveAbility = nullptr;
			abilityPtr->InputReleased();
		}
	}
}