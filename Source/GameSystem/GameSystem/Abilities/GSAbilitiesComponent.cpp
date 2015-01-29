// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "Net/UnrealNetwork.h"

#include "GSAbilitiesComponent.h"

UGSAbilitiesComponent::UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UGSAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
void UGSAbilitiesComponent::InputPressed(int32 SetIndex, int32 SlotIndex)
{
	UGASAbilitiesComponent::InputPressed(AbilitySets[SetIndex].AbilitySlots[SlotIndex].AbilityIndex);
}
void UGSAbilitiesComponent::InputReleased(int32 SetIndex, int32 SlotIndex)
{
	UGASAbilitiesComponent::InputReleased(AbilitySets[SetIndex].AbilitySlots[SlotIndex].AbilityIndex);
}

void UGSAbilitiesComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGSAbilitiesComponent, OwnedAbilities, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGSAbilitiesComponent, AbilitySets, COND_OwnerOnly);
}

void UGSAbilitiesComponent::InitializeActivatableAbilities()
{
	int32 SetCounter = 0;
	for (FGSActiveAbilitiesSlotConfig& set : ActivatableAbilitiesConfig)
	{
		FGSAbilitiesSets setIn;
		setIn.SetIndex = SetCounter;
		for (int32 Index = 0; Index < set.NumberOfSlots; Index++)
		{
			FGSAbilitySlot ability;
			ability.SlotIndex = Index;
			ability.SetIndex = SetCounter;
			setIn.AbilitySlots.Add(ability);
			
		}
		AbilitySets.Add(setIn);
		SetCounter++;
	}
}