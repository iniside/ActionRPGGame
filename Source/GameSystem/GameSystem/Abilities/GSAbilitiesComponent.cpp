// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSAbility.h"

#include "Net/UnrealNetwork.h"
#include "Widgets/GSAbilityContainerWidget.h"
#include "Widgets/GSAbilityCastTimeWidget.h"
#include "GSAbilitiesComponent.h"

UGSAbilitiesComponent::UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	CurrentAbility = -1;
}

void UGSAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializeActivatableAbilities();
	if (AbilityContainerClass)
	{
		AbilityContainer = CreateWidget<UGSAbilityContainerWidget>(GetWorld(), AbilityContainerClass);
		if (AbilityContainer)
		{
			AbilityContainer->AbilityComponent = this;
			AbilityContainer->SetVisibility(ActionBarVisibility);

			AbilityContainer->AbilityTabClass = AbilityTabClass;
			AbilityContainer->AbilitySlotClass = AbilitySlotClass;
			AbilityContainer->AbilityWidgetClass = AbilityWidgetClass;
			AbilityContainer->DropSlotName = ActionBarAbilitySlotName;
			AbilityContainer->InitializeWidget();
		}

		CastTimeWidget = CreateWidget<UGSAbilityCastTimeWidget>(GetWorld(), CastTimeWidgetClass);
		if (CastTimeWidget)
		{
			CastTimeWidget->AbilityComponent = this;

			CastTimeWidget->InitializeWidget();
		}
	}
	OnAbilityAddedToSet.ExecuteIfBound();
}
void UGSAbilitiesComponent::OnRep_OwnedAbilities()
{
	OnAbilityAddedToSet.ExecuteIfBound();
}
float UGSAbilitiesComponent::GetCurrentCastTime()
{
	if (CurrentAbility > -1)
	{
		return GetGSAbility(CurrentAbility)->CurrentCastTime;
	}
	return 0;
}
float UGSAbilitiesComponent::GetCastTime()
{
	if (CurrentAbility > -1)
	{
		return GetGSAbility(CurrentAbility)->CastTime;
	}
	return 0;
}
void UGSAbilitiesComponent::InputPressed(int32 SetIndex, int32 SlotIndex)
{
	CurrentAbility = AbilitySets[SetIndex].AbilitySlots[SlotIndex].AbilityIndex;
	if (CurrentAbility < 0)
		return;
	UGASAbilitiesComponent::InputPressed(CurrentAbility);
	OnGetAbilityIndex.ExecuteIfBound(CurrentAbility);
	OnAbilityPressedIndex.Broadcast(CurrentAbility);
	if (InstancedAbilities[CurrentAbility].ActiveAbility
		&& !InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.IsBound())
	{
		InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.BindUObject(this, &UGSAbilitiesComponent::Del_OnAbilityCasted);
	}
}
void UGSAbilitiesComponent::InputReleased(int32 SetIndex, int32 SlotIndex)
{
	UGASAbilitiesComponent::InputReleased(AbilitySets[SetIndex].AbilitySlots[SlotIndex].AbilityIndex);
}

void UGSAbilitiesComponent::GiveAbilityAndInsert(TSubclassOf<class  UGSAbility> AbilityIn)
{
	int32 abIndex = AddAbilityToActiveList(AbilityIn);

	if (abIndex != -1)
	{
		for (FGSAbilitiesSets& set : AbilitySets)
		{
			for (FGSAbilitySlot& slot : set.AbilitySlots)
			{
				if (slot.AbilityIndex == -1)
				{
					slot.AbilityIndex = abIndex;
					OnAbilityAddedToSet.ExecuteIfBound();
					return;
				}
			}
		}
	}
	
}

void UGSAbilitiesComponent::BP_AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex, int32 AbilityIndex)
{
	AddAbilityToSlot(TargetSetIndex, TargetSlotIndex, AbilityIndex);
}
void UGSAbilitiesComponent::AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex, int32 AbilityIndex)
{
	//we simply override index, it will require more checking though!
	AbilitySets[TargetSetIndex].AbilitySlots[TargetSlotIndex].AbilityIndex = AbilityIndex;

	OnAbilityAddedToSet.ExecuteIfBound();
}
void UGSAbilitiesComponent::AddAbilityToSlot(int32 TargetSetIndex, int32 TargetSlotIndex,
	int32 LastSetIndex, int32 LastSlotIndex, int32 AbilityIndex)
{

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
void UGSAbilitiesComponent::Del_OnAbilityCasted()
{
	if (CastTimeWidget)
	{
		CastTimeWidget->MarkWidgetDirty();
	}
	if (InstancedAbilities[CurrentAbility].ActiveAbility)
	{
		InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.Unbind();
	}
}

class UGSAbility* UGSAbilitiesComponent::GetGSAbility(int32 IndexIn)
{
	return Cast<UGSAbility>(InstancedAbilities[IndexIn].ActiveAbility);
}