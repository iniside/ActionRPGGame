// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityManagerComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Layout/Visibility.h"

#include "DWBPFunctionLibrary.h"
#include "SDraggableWindowWidget.h"
#include "ARPlayerController.h"

// Sets default values for this component's properties
UARAbilityManagerComponent::UARAbilityManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
		// ...
	}
}


// Called every frame
void UARAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARAbilityManagerComponent::ShowHideAbilityManager()
{
}

void UARAbilityManagerComponent::SetCurrentSet(int32 SetIndex)
{
	//add server/client checks.
	SelectGroup(AMIntToEnum<EAMGroup>(SetIndex));

}

void UARAbilityManagerComponent::OnGroupSelectionConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess)
{
	EAMGroup Group = ValidGroup; //ActiveGroup

	UAFAbilityComponent* AbilityComp = GetAbilityComponent();
	if (!AbilityComp)
		return;

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		TArray<FAFAbilityActionSet> AbilityActionSet;
		TArray<FAFOnAbilityReady> InputReadyDelegates;
		for (int32 Idx = 0; Idx < AbilityTagsSet[AMEnumToInt<EAMGroup>(ValidGroup)].Num(); Idx++)
		{
			TArray<FGameplayTag> WeaponInput = GetInputTag(Group, AMIntToEnum<EAMSlot>(Idx));
			TSoftClassPtr<UGAAbilityBase> NextWeaponAbility = GetAbilityTag(Group, AMIntToEnum<EAMSlot>(Idx));

			FAFAbilityActionSet Set;
			Set.AbilityInputs = WeaponInput;
			Set.AbilityTag = NextWeaponAbility;

			FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARAbilityManagerComponent::OnInputReady,
				NextWeaponAbility, WeaponInput);

			AbilityActionSet.Add(Set);
			InputReadyDelegates.Add(ReadyDelegate);
		}

		AbilityComp->SetAbilitiesToActions(AbilityActionSet, InputReadyDelegates);
	}
	else
	{
		TArray<FAFAbilityActionSet> AbilityActionSet;
		TArray<FAFOnAbilityReady> InputReadyDelegates;

		for (int32 Idx = 0; Idx < AbilityTagsSet[AMEnumToInt<EAMGroup>(ValidGroup)].Num(); Idx++)
		{
			TArray<FGameplayTag> WeaponInput = GetInputTag(Group, AMIntToEnum<EAMSlot>(Idx));
			TSoftClassPtr<UGAAbilityBase> NextWeaponAbility = GetAbilityTag(Group, AMIntToEnum<EAMSlot>(Idx));

			FAFAbilityActionSet Set;
			Set.AbilityInputs = WeaponInput;
			Set.AbilityTag = NextWeaponAbility;

			FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARAbilityManagerComponent::OnInputReady,
				NextWeaponAbility, WeaponInput);

			AbilityActionSet.Add(Set);
			InputReadyDelegates.Add(ReadyDelegate);
		}
		AbilityComp->SetAbilitiesToActions(AbilityActionSet, InputReadyDelegates);
		OnAbilitySetChanged.Broadcast(ValidGroup);
	}
}

void UARAbilityManagerComponent::OnInputReady(TSoftClassPtr<UGAAbilityBase> WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
{
	OnAbilitySetChanged.Broadcast(ActiveGroup);
}