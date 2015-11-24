// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSAbility.h"
#include "../GSPlayerController.h"
#include "UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GSAbilityContainerWidget.h"
#include "Widgets/GSAbilityCastTimeWidget.h"

#include "Widgets/GSAbilityBookContainerWidget.h"

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
	InitializeAbilityBook();
}
void UGSAbilitiesComponent::InitializeGUI()
{
	if (AbilityContainerClass)
	{
		AbilityContainer = CreateWidget<UGSAbilityContainerWidget>(PCOwner, AbilityContainerClass);
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
	}
	if (CastTimeWidgetClass)
	{
		CastTimeWidget = CreateWidget<UGSAbilityCastTimeWidget>(PCOwner, CastTimeWidgetClass);
		if (CastTimeWidget)
		{
			CastTimeWidget->AbilityComponent = this;

			CastTimeWidget->InitializeWidget();
		}
	}
	if (AbilityBookConfig.AbilityBookContainerClass)
	{
		AbilityBookContainer = CreateWidget<UGSAbilityBookContainerWidget>(PCOwner, AbilityBookConfig.AbilityBookContainerClass);
		if (AbilityBookContainer)
		{
			AbilityBookContainer->InitializeWidget(AbilityBookConfig, this);
		}
	}
	//if (GetNetMode() == ENetMode::NM_Standalone)
	//{
		OnAbilityAddedToSet.ExecuteIfBound();
		OnAbilityAddedToBook.ExecuteIfBound();
	//}
}

void UGSAbilitiesComponent::OnRep_OwnedAbilities()
{
	OnAbilityAddedToBook.ExecuteIfBound();
}
void UGSAbilitiesComponent::OnRep_AbilitySets()
{
	OnAbilityAddedToSet.ExecuteIfBound();
}
float UGSAbilitiesComponent::GetCurrentCastTime()
{
	if (CurrentAbility > -1)
	{
		//return GetGSAbility(CurrentAbility)->CurrentCastTime;
	}
	return 0;
}
float UGSAbilitiesComponent::GetCastTime()
{
	if (CurrentAbility > -1)
	{
		//return GetGSAbility(CurrentAbility)->CastTime;
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
	//if (InstancedAbilities[CurrentAbility].ActiveAbility
	//	&& !InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.IsBound())
	//{
	//	InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.BindUObject(this, &UGSAbilitiesComponent::Del_OnAbilityCasted);
	//}
}
void UGSAbilitiesComponent::InputReleased(int32 SetIndex, int32 SlotIndex)
{
	UGASAbilitiesComponent::InputReleased(AbilitySets[SetIndex].AbilitySlots[SlotIndex].AbilityIndex);
}

void UGSAbilitiesComponent::GiveAbilityAndInsert(TSubclassOf<class  UGSAbility> AbilityIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerGiveAbilityAndInsert(AbilityIn);
	}
	else
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
						set.RepMe++;
						slot.AbilityIndex = abIndex;
						if (GetNetMode() == ENetMode::NM_Standalone)
							OnAbilityAddedToSet.ExecuteIfBound();
						
						return;
					}
				}
			}
		}
	}
}
void UGSAbilitiesComponent::ServerGiveAbilityAndInsert_Implementation(TSubclassOf<class  UGSAbility> AbilityIn)
{
	GiveAbilityAndInsert(AbilityIn);
}
bool UGSAbilitiesComponent::ServerGiveAbilityAndInsert_Validate(TSubclassOf<class  UGSAbility> AbilityIn)
{
	return true;
}
void UGSAbilitiesComponent::GiveAbility(TSubclassOf<class  UGSAbility> AbilityIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerGiveAbility(AbilityIn);
	}
	else
	{
		//ok this is very basic implementation, which will simply add ability to the first available slot.
		for (FGSAbilityBookTab& tab : OwnedAbilities.AbilitiesTab)
		{
			for (FGSAbilityBookSlot& slot : tab.Abilities)
			{
				if (slot.AbilityClass == nullptr)
				{
					slot.AbilityClass = AbilityIn;
					slot.bIsAbilityActive = true;
					slot.bIsAbilityVisible = true;
					break;
				}
			}
		}
		//make sure changes are replicated back.
		OwnedAbilities.ForceRep++;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnRep_OwnedAbilities();
	}
}
void UGSAbilitiesComponent::ServerGiveAbility_Implementation(TSubclassOf<class  UGSAbility> AbilityIn)
{
	GiveAbility(AbilityIn);
}
bool UGSAbilitiesComponent::ServerGiveAbility_Validate(TSubclassOf<class  UGSAbility> AbilityIn)
{
	return true;
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
void UGSAbilitiesComponent::AddAbilityToSlotFromBook(int32 TargetSetIndex, int32 TargetSlotIndex, int32 BookTabIndex,
	int32 BookSlotIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddAbilityToSlotFromBook(TargetSetIndex, TargetSlotIndex, BookTabIndex, BookSlotIndex);
	}
	else
	{
		TSubclassOf<UGSAbility> abilityClass = OwnedAbilities.AbilitiesTab[BookTabIndex].Abilities[BookSlotIndex].AbilityClass;

		int32 abIndex = AddAbilityToActiveList(abilityClass);

		AbilitySets[TargetSetIndex].AbilitySlots[TargetSetIndex].AbilityIndex = abIndex;
		AbilitySets[TargetSetIndex].RepMe++;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnRep_AbilitySets();
	}
}
void UGSAbilitiesComponent::ServerAddAbilityToSlotFromBook_Implementation(int32 TargetSetIndex, int32 TargetSlotIndex,
	int32 BookTabIndex, int32 BookSlotIndex)
{
	AddAbilityToSlotFromBook(TargetSetIndex, TargetSlotIndex, BookTabIndex, BookSlotIndex);
}
bool UGSAbilitiesComponent::ServerAddAbilityToSlotFromBook_Validate(int32 TargetSetIndex, int32 TargetSlotIndex,
	int32 BookTabIndex, int32 BookSlotIndex)
{
	return true;
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
void UGSAbilitiesComponent::InitializeAbilityBook()
{
	int32 SetCounter = 0;
	for (FGSAbilityBookConfig& set : AbilityBookSlotsConfig)
	{
		FGSAbilityBookTab setIn;
		setIn.TabIndex = SetCounter;
		for (int32 Index = 0; Index < set.SlotNumber; Index++)
		{
			FGSAbilityBookSlot ability(Index, SetCounter);
			setIn.Abilities.Add(ability);

		}
		OwnedAbilities.AbilitiesTab.Add(setIn);
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
		//InstancedAbilities[CurrentAbility].ActiveAbility->OnAbilityCastedDel.Unbind();
	}
}

class UGSAbility* UGSAbilitiesComponent::GetGSAbility(int32 IndexIn)
{
	return Cast<UGSAbility>(InstancedAbilities[IndexIn].ActiveAbility);
}