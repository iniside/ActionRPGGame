// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbility.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "IGIPawn.h"

#include "GASAbilitiesComponent.h"

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
		//no ability on client. No RPC.
		if (InstancedAbilities[AbilityId].ActiveAbility)
		{
			InstancedAbilities[AbilityId].ActiveAbility->OnAbilityExecutedNative();
			ServerInputPressed(AbilityId);
		}
		//right now we will use simple prediction, which means just run ability on client
		//and then use RPCs/repnotifies from server to controll it state on clien,
		//so we won't get out of sync to much.

	}
	else
	{
		if (InstancedAbilities[AbilityId].ActiveAbility)
			InstancedAbilities[AbilityId].ActiveAbility->OnAbilityExecutedNative();
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
		ServerInputReleased(AbilityId);
	}
	else
	{

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

void UGASAbilitiesComponent::BP_AddAbility(TSubclassOf<class UGASAbility> AbilityClass)
{
	AddAbilityToActiveList(AbilityClass);
}
int32 UGASAbilitiesComponent::AddAbilityToActiveList(TSubclassOf<class UGASAbility> AbilityClass)
{
	if (AbilityClass)
	{
		UGASAbility* ability = NewObject<UGASAbility>(GetOwner(), AbilityClass);
		int32 slotCounter = 0;
		for (FGASActiveAbilitySlot& ab : InstancedAbilities)
		{
			if (ab.ActiveAbility == nullptr)
			{
				ability->AbilityComponent = this;
				if (PawnInterface)
				{
					ability->POwner = PawnInterface->GetGamePawn();
					ability->PCOwner = PawnInterface->GetGamePlayerController();
					//ability->AIOwner = PawnInterface->GetGameController();
				}
				ability->Initialize();
				ab.ActiveAbility = ability;
				return slotCounter;
			}
			slotCounter++;
		}
	}
	return -1;
}

void UGASAbilitiesComponent::BP_GiveAbility(TSubclassOf<class UGASAbility> AbilityClass)
{
	GiveAbility(AbilityClass);
}

void UGASAbilitiesComponent::GiveAbility(TSubclassOf<class UGASAbility> AbilityClass)
{

}

void UGASAbilitiesComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGASAbilitiesComponent, InstancedAbilities);
}
bool UGASAbilitiesComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const FGASActiveAbilitySlot& ability : InstancedAbilities)
	{
		if (ability.ActiveAbility)
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASAbility*>(ability.ActiveAbility), *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}
void UGASAbilitiesComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{

}

void UGASAbilitiesComponent::OnRep_InstancedAbilities()
{
	for (FGASActiveAbilitySlot& ab : InstancedAbilities)
	{
		if (ab.ActiveAbility && !ab.ActiveAbility->GetIsInitialized())
		{
			ab.ActiveAbility->AbilityComponent = this;
			if (PawnInterface)
			{
				ab.ActiveAbility->POwner = PawnInterface->GetGamePawn();
				ab.ActiveAbility->PCOwner = PawnInterface->GetGamePlayerController();
			}
			ab.ActiveAbility->Initialize();
		}
	}
}

void UGASAbilitiesComponent::InitializeInstancedAbilities()
{
	for (int32 Index = 0; Index < InstancedAbilitiesConfig.MaxAbilities; Index++)
	{
		FGASActiveAbilitySlot ActiveAbility;
		ActiveAbility.SlotIndex = Index;
		InstancedAbilities.Add(ActiveAbility);
	}
}