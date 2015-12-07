// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbilityBase.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "IGIPawn.h"

#include "GASAbilitiesComponent.h"

void FGASActiveAbility::Reset()
{
	SetIndex = -1;
	SlotIndex = -1;
	ActiveAbility->MarkPendingKill();
	ActiveAbility = nullptr;
}

void FGASActiveAbilityContainer::RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex)
{
	if (AbilitySets.IsValidIndex(SetIndex))
	{
		if (AbilitySets[SetIndex].Abilities.IsValidIndex(SlotIndex))
		{
			if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbility)
			{
				if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbility->StaticClass() == AbilityClass)
				{
					AbilitySets[SetIndex].Abilities[SlotIndex].Reset();
				}
			}
		}
	}
}

UGASAbilitiesComponent::UGASAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bIsAnyAbilityActive = false;
}
bool UGASAbilitiesComponent::CanActivateAbility()
{
	//if there are no activate abilities, we can activate the selected one.
	//this should be configarable ie. how many abilities can be activated at the same time ?
	if (!bIsAnyAbilityActive)
	{
		return true;
	}
	return true;
}
void UGASAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();
	PawnInterface = Cast<IIGIPawn>(GetOwner());

	InitializeInstancedAbilities();
}

void UGASAbilitiesComponent::InputPressed(int32 AbilityId)
{
	//if we can't activate ability there is no reason to go further.
	if (AbilityId < 0)
		return;
	if (!CanActivateAbility())
		return;
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		if (InstancedAbilities.Num() <= 0)
			return;
		//no ability on client. No RPC.
		if (InstancedAbilities[AbilityId].ActiveAbility 
			&& InstancedAbilities[AbilityId].ActiveAbility->CanUseAbility())
		{
			//waiting for confirmation is exclusive at least now, ability either wait or not.
			if (InstancedAbilities[AbilityId].ActiveAbility->IsWaitingForConfirm())
			{
				InstancedAbilities[AbilityId].ActiveAbility->ConfirmAbility();
			}
			else
			{
				InstancedAbilities[AbilityId].ActiveAbility->OnNativeInputPressed();
				ServerInputPressed(AbilityId);
			}
		}
		//right now we will use simple prediction, which means just run ability on client
		//and then use RPCs/repnotifies from server to controll it state on clien,
		//so we won't get out of sync to much.

	}
	else
	{
		if (InstancedAbilities.Num() <= 0)
			return;

		if (InstancedAbilities[AbilityId].ActiveAbility
			&& InstancedAbilities[AbilityId].ActiveAbility->CanUseAbility())
		{
			if (InstancedAbilities[AbilityId].ActiveAbility->IsWaitingForConfirm())
			{
				InstancedAbilities[AbilityId].ActiveAbility->ConfirmAbility();
			}
			else
			{
				InstancedAbilities[AbilityId].ActiveAbility->OnNativeInputPressed();
			}
		}

	}
}
void UGASAbilitiesComponent::ServerInputPressed_Implementation(int32 AbilityId)
{
	InputPressed(AbilityId);
}
bool UGASAbilitiesComponent::ServerInputPressed_Validate(int32 AbilityId)
{
	return true;
}


void UGASAbilitiesComponent::InputReleased(int32 AbilityId)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		if (InstancedAbilities.Num() <= 0)
			return;

		if (InstancedAbilities[AbilityId].ActiveAbility
			&& InstancedAbilities[AbilityId].ActiveAbility->CanUseAbility())
		{
			if(!InstancedAbilities[AbilityId].ActiveAbility->IsWaitingForConfirm())
				InstancedAbilities[AbilityId].ActiveAbility->OnNativeInputReleased();
		}
		ServerInputReleased(AbilityId);
	}
	else
	{
		if (InstancedAbilities.Num() <= 0)
			return;
		if (InstancedAbilities[AbilityId].ActiveAbility
			&& InstancedAbilities[AbilityId].ActiveAbility->CanUseAbility())
		{
			if (!InstancedAbilities[AbilityId].ActiveAbility->IsWaitingForConfirm())
				InstancedAbilities[AbilityId].ActiveAbility->OnNativeInputReleased();
		}
	}
}
void UGASAbilitiesComponent::ServerInputReleased_Implementation(int32 AbilityId)
{
	InputReleased(AbilityId);
}
bool UGASAbilitiesComponent::ServerInputReleased_Validate(int32 AbilityId)
{
	return true;
}

void UGASAbilitiesComponent::BP_AddAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	AddAbilityToActiveList(AbilityClass);
}
void UGASAbilitiesComponent::RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	int32 num = InstancedAbilities.Num();
	
	
	bool found = false;
	while (!found)
	{
		int32 randIdx = FMath::RandRange(0, num - 1);
		if (InstancedAbilities[randIdx].ActiveAbility->StaticClass() == AbilityClass)
		{
			InstancedAbilities.RemoveAt(randIdx);
			found = true;
		}
	}
}
void UGASAbilitiesComponent::BP_RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	RemoveAbilityFromActiveList(AbilityClass);
}
int32 UGASAbilitiesComponent::AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	if (AbilityClass)
	{
		UGASAbilityBase* ability = NewObject<UGASAbilityBase>(GetOwner(), AbilityClass);
		int32 slotCounter = 0;
		for (FGASActiveAbility& ab : InstancedAbilities)
		{
			if (ab.ActiveAbility == nullptr)
			{
				ability->OwningComp = this;
				if (PawnInterface)
				{
					ability->POwner = PawnInterface->GetGamePawn();
					ability->PCOwner = PawnInterface->GetGamePlayerController();
					ability->OwnerCamera = PawnInterface->GetPawnCamera();
					//ability->AIOwner = PawnInterface->GetGameController();
				}
				ab.ActiveAbility = ability;
				ability->InitAbility();
				return slotCounter;
			}
			slotCounter++;
		}
	}
	return -1;
}

void UGASAbilitiesComponent::AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex)
{
	if (!AbilityClass)
		return;
	UGASAbilityBase* ability = NewObject<UGASAbilityBase>(GetOwner(), AbilityClass);
	ability->OwningComp = this;
	if (PawnInterface)
	{
		ability->POwner = PawnInterface->GetGamePawn();
		ability->PCOwner = PawnInterface->GetGamePlayerController();
		ability->OwnerCamera = PawnInterface->GetPawnCamera();
		//ability->AIOwner = PawnInterface->GetGameController();
	}
	ActiveAbilityContainer.AddAbility(ability, SetIndex, SlotIndex);
	ability->InitAbility();
}

void UGASAbilitiesComponent::RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex)
{
	ActiveAbilityContainer.RemoveAbility(AbilityClass, SetIndex, SlotIndex);
}

void UGASAbilitiesComponent::BP_GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	GiveAbility(AbilityClass);
}

void UGASAbilitiesComponent::GiveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{

}

void UGASAbilitiesComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGASAbilitiesComponent, InstancedAbilities);
	DOREPLIFETIME(UGASAbilitiesComponent, ActiveAbilityContainer);
}

bool UGASAbilitiesComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const FGASActiveAbility& ability : InstancedAbilities)
	{
		if (ability.ActiveAbility)
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASAbilityBase*>(ability.ActiveAbility), *Bunch, *RepFlags);
		}
	}
	for (const FGASActiveAbilitySet& Set : ActiveAbilityContainer.AbilitySets)
	{
		for (const FGASActiveAbility& Ability : Set.Abilities)
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASAbilityBase*>(Ability.ActiveAbility), *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}
void UGASAbilitiesComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{

}

void UGASAbilitiesComponent::OnRep_InstancedAbilities()
{
	for (FGASActiveAbility& ab : InstancedAbilities)
	{
		if (ab.ActiveAbility)// && !ab.ActiveAbility->GetIsInitialized())
		{
			ab.ActiveAbility->OwningComp = this;
			if (PawnInterface)
			{
				ab.ActiveAbility->POwner = PawnInterface->GetGamePawn();
				ab.ActiveAbility->PCOwner = PawnInterface->GetGamePlayerController();
				ab.ActiveAbility->OwnerCamera = PawnInterface->GetPawnCamera();

				//ability->AIOwner = PawnInterface->GetGameController();
			}
			ab.ActiveAbility->InitAbility();
		}
	}
}

void UGASAbilitiesComponent::InitializeInstancedAbilities()
{
	int32 SetIdx = 0;
	for (FGASAbilitySetConfig& Set : InstancedAbilitiesConfig.Sets)
	{
		FGASActiveAbilitySet AbSet;
		for (int32 Idx = 0; Idx < Set.MaxAbilities; Idx++)
		{
			FGASActiveAbility ab;
			ab.SetIndex = SetIdx;
			ab.SlotIndex = Idx;
			AbSet.Abilities.Add(ab);
		}
		SetIdx++;
	}
	for (int32 Index = 0; Index < InstancedAbilitiesConfig.MaxAbilities; Index++)
	{
		FGASActiveAbility ActiveAbility;
		ActiveAbility.SlotIndex = Index;
		InstancedAbilities.Add(ActiveAbility);
	}
}