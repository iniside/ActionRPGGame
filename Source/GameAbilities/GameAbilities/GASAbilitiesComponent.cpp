// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbilityBase.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GASInputOverride.h"
#include "GASGlobals.h"
#include "GASAbilitySet.h"
#include "IGIPawn.h"

#include "GASAbilitiesComponent.h"

void FGASActiveAbility::SetAbility(class UGASAbilityBase* Ability, int32 SetIndexIn, int32 SlotIndexIn, int32 SubSlotIndex)
{
	if (ActiveAbilities[SubSlotIndex])
	{
		ActiveAbilities[SubSlotIndex]->MarkPendingKill();
	}
	ActiveAbilities[SubSlotIndex] = nullptr;
	ActiveAbilities[SubSlotIndex] = Ability;
	SetIndex = SetIndexIn;
	SlotIndex = SlotIndexIn;
}

void FGASActiveAbility::Reset(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	int32 AbilityNum = ActiveAbilities.Num();
	for (int32 Idx = 0; Idx < AbilityNum; Idx++)
	{
		if (ActiveAbilities[Idx] && ActiveAbilities[Idx]->StaticClass() == AbilityClass)
		{
			ActiveAbilities[Idx]->MarkPendingKill();
			ActiveAbilities[Idx] = nullptr;
		}
	}
}

void FGASActiveAbilityContainer::RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex)
{
	if (AbilitySets.IsValidIndex(SetIndex))
	{
		if (AbilitySets[SetIndex].Abilities.IsValidIndex(SlotIndex))
		{
			AbilitySets[SetIndex].Abilities[SlotIndex].Reset(AbilityClass);
		}
	}
}
bool FGASActiveAbilityContainer::CanUseAbility(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]
		&& AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]->CanUseAbility())
	{
		return true;
	}
	return false;
}
bool FGASActiveAbilityContainer::IsWaitingForConfirm(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]
		&& AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]->IsWaitingForConfirm())
	{
		return true;
	}
	return false;
}
void FGASActiveAbilityContainer::ConfirmAbility(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]->ConfirmAbility();
}
void FGASActiveAbilityContainer::OnNativeInputPressed(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]->OnNativeInputPressed();
}
void FGASActiveAbilityContainer::OnNativeInputReleased(int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[SubSlotIndex]->OnNativeInputReleased();
}

void FGASActiveAbilityContainer::ClearAndResizeAbilitiesCount(int32 SetIndex, int32 SlotIndex, int32 NewSize)
{
	int32 MaxAbilities = AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities.Num();
	for (int32 Idx = 0; Idx < MaxAbilities; Idx++)
	{
		if (AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[Idx])
		{
			AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[Idx]->MarkPendingKill();
			AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[Idx] = nullptr;
		}
	}
	//AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities.Shrink();
	//AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities.SetNum(NewSize);
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
	if (!bIsAnyAbilityActive && !ExecutingAbility)
	{
		return true;
	}
	return false;
}
void UGASAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();
	PawnInterface = Cast<IIGIPawn>(GetOwner());

	InitializeInstancedAbilities();
}
void UGASAbilitiesComponent::BP_InputPressed(int32 SetIndex, int32 SlotIndex)
{
	NativeInputPressed(SetIndex, SlotIndex);
}
void UGASAbilitiesComponent::NativeInputPressed(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	if (SetIndex < 0 || SlotIndex < 0)
		return;
	if (!CanActivateAbility())
		return;
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		//If this set have custom input, let it handle it completly.
		if (ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride)
		{
			ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride->NativeInputPressed(SlotIndex);
			return;
		}
		if (!ActiveAbilityContainer.IsValid(SetIndex, SlotIndex))
			return;
		//no ability on client. No RPC.
		if (ActiveAbilityContainer.CanUseAbility(SetIndex, SlotIndex))
		{
			//waiting for confirmation is exclusive at least now, ability either wait or not.
			if (ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, SlotIndex))
			{
				ActiveAbilityContainer.ConfirmAbility(SetIndex, SlotIndex);
			}
			else
			{
				ActiveAbilityContainer.OnNativeInputPressed(SetIndex, SlotIndex);
				ServerNativeInputPressed(SetIndex, SlotIndex, AbilityIndex);
			}
		}
		//right now we will use simple prediction, which means just run ability on client
		//and then use RPCs/repnotifies from server to controll it state on clien,
		//so we won't get out of sync to much.

	}
	else
	{
		if (ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride)
		{
			ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride->NativeInputPressed(SlotIndex);
			return;
		}
		if (!ActiveAbilityContainer.IsValid(SetIndex, SlotIndex))
			return;
		//no ability on client. No RPC.
		if (ActiveAbilityContainer.CanUseAbility(SetIndex, SlotIndex))
		{
			//waiting for confirmation is exclusive at least now, ability either wait or not.
			if (ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, SlotIndex))
			{
				ActiveAbilityContainer.ConfirmAbility(SetIndex, SlotIndex);
			}
			else
			{
				ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[AbilityIndex]->OnNativeInputPressed();
				//ActiveAbilityContainer.OnNativeInputPressed(SetIndex, SlotIndex);
			}
		}

	}
}

void UGASAbilitiesComponent::ServerNativeInputPressed_Implementation(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	NativeInputPressed(SetIndex, SlotIndex, AbilityIndex);
}
bool UGASAbilitiesComponent::ServerNativeInputPressed_Validate(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	return true;
}

void UGASAbilitiesComponent::BP_InputReleased(int32 SetIndex, int32 SlotIndex)
{
	NativeInputReleased(SetIndex, SlotIndex);
}

void UGASAbilitiesComponent::NativeInputReleased(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	if (!CanActivateAbility())
		return;
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		if (!ActiveAbilityContainer.IsValid(SetIndex, SlotIndex))
			return;

		if (ActiveAbilityContainer.CanUseAbility(SetIndex, SlotIndex))
		{
			if (!ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, SlotIndex))
				ActiveAbilityContainer.OnNativeInputReleased(SetIndex, SlotIndex);
		}
		ServerNativeInputReleased(SetIndex, SlotIndex, AbilityIndex);
	}
	else
	{
		if (ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride)
		{
			ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride->NativeInputReleased(SlotIndex);
			return;
		}

		if (!ActiveAbilityContainer.IsValid(SetIndex, SlotIndex))
			return;

		if (ActiveAbilityContainer.CanUseAbility(SetIndex, SlotIndex))
		{
			if (!ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, SlotIndex))
				ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[SlotIndex].ActiveAbilities[AbilityIndex]->OnNativeInputReleased();

			//	ActiveAbilityContainer.OnNativeInputReleased(SetIndex, SlotIndex);
		}
	}
}

void UGASAbilitiesComponent::ServerNativeInputReleased_Implementation(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	NativeInputReleased(SetIndex, SlotIndex, AbilityIndex);
}
bool UGASAbilitiesComponent::ServerNativeInputReleased_Validate(int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	return true;
}

void UGASAbilitiesComponent::BP_AddAbility2(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	AddAbilityToActiveList(AbilityClass, SetIndex, SlotIndex, AbilityIndex);
}
void UGASAbilitiesComponent::BP_AddAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	AddAbilityToActiveList(AbilityClass);
}
void UGASAbilitiesComponent::RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
}

void UGASAbilitiesComponent::BP_RemoveAbility(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	RemoveAbilityFromActiveList(AbilityClass);
}
int32 UGASAbilitiesComponent::AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass)
{
	return -1;
}

void UGASAbilitiesComponent::AddAbilityToActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 AbilityIndex)
{
	if (!AbilityClass)
		return;
	InstanceAbility(AbilityClass, SetIndex, SlotIndex, AbilityIndex);
}

void UGASAbilitiesComponent::RemoveAbilityFromActiveList(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex)
{
	ActiveAbilityContainer.RemoveAbility(AbilityClass, SetIndex, SlotIndex);
}
void UGASAbilitiesComponent::InstanceAbility(TSubclassOf<class UGASAbilityBase> AbilityClass, int32 SetIndex, int32 SlotIndex, int32 SubSlotIndex)
{
	if (AbilityClass)
	{
		UGASAbilityBase* ability = NewObject<UGASAbilityBase>(GetOwner(), AbilityClass);
		ability->OwningComp = this;
		ability->AbilityComponent = this;
		if (PawnInterface)
		{
			ability->POwner = PawnInterface->GetGamePawn();
			ability->PCOwner = PawnInterface->GetGamePlayerController();
			ability->OwnerCamera = PawnInterface->GetPawnCamera();
			//ability->AIOwner = PawnInterface->GetGameController();
		}
		ability->InitAbility();
		ActiveAbilityContainer.AddAbility(ability, SetIndex , SlotIndex, SubSlotIndex);
	}
}

void UGASAbilitiesComponent::NativeAddAbilitiesFromSet(TSubclassOf<class UGASAbilitySet> AbilitySet, int32 SetIndex)
{
	//do not let add abilities on non authority.
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
		return;

	UGASAbilitySet* Set = AbilitySet.GetDefaultObject();
	ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride = NewObject<UGASInputOverride>(this, Set->AbilitySet.CustomInputClass);
	ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride->AbilityComp = this;
	ActiveAbilityContainer.AbilitySets[SetIndex].InputOverride->SetIndex = SetIndex;
	int32 AbilityIdx = 0;
	int32 SubAbilityIdx = 0;
	for (const FGASAbilitySetItem& Item : Set->AbilitySet.Abilities)
	{
		ActiveAbilityContainer.ClearAndResizeAbilitiesCount(SetIndex, AbilityIdx, Item.AbilityClass.Num());
		for (TSubclassOf<UGASAbilityBase> AbilityClass : Item.AbilityClass)
		{
			InstanceAbility(AbilityClass, SetIndex, AbilityIdx, SubAbilityIdx);
			SubAbilityIdx++;
		}
		SubAbilityIdx = 0;
		AbilityIdx++;
	}
}

void UGASAbilitiesComponent::BP_AddAbilitiesFromSet(TSubclassOf<class UGASAbilitySet> AbilitySet, int32 SetIndex)
{
	NativeAddAbilitiesFromSet(AbilitySet, SetIndex);
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
	DOREPLIFETIME(UGASAbilitiesComponent, ActiveAbilityContainer);
	DOREPLIFETIME_CONDITION(UGASAbilitiesComponent, RepMontage, COND_SkipOwner);
	//DOREPLIFETIME(UGASAbilitiesComponent, RepMontage);
}

bool UGASAbilitiesComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const FGASActiveAbilitySet& Set : ActiveAbilityContainer.AbilitySets)
	{
		if (Set.InputOverride)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASInputOverride*>(Set.InputOverride), *Bunch, *RepFlags);
		
		for (const FGASActiveAbility& ability : Set.Abilities)
		{
			for (const UGASAbilityBase* Ability : ability.ActiveAbilities)
			{
				if(Ability)
					WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASAbilityBase*>(Ability), *Bunch, *RepFlags);
			}
		}
	}
	return WroteSomething;
}
void UGASAbilitiesComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{

}

void UGASAbilitiesComponent::OnRep_InstancedAbilities()
{
	for (FGASActiveAbilitySet& Set : ActiveAbilityContainer.AbilitySets)
	{
		for (FGASActiveAbility& AbilitySlot : Set.Abilities)
		{
			for (UGASAbilityBase* Ability : AbilitySlot.ActiveAbilities)
			{
				if (Ability)// && !ab.ActiveAbility->GetIsInitialized())
				{
					Ability->OwningComp = this;
					if (PawnInterface)
					{
						Ability->POwner = PawnInterface->GetGamePawn();
						Ability->PCOwner = PawnInterface->GetGamePlayerController();
						Ability->OwnerCamera = PawnInterface->GetPawnCamera();

						//ability->AIOwner = PawnInterface->GetGameController();
					}
					Ability->AbilityComponent = this;
					Ability->InitAbility();
				}
			}
		}
	}
}

void UGASAbilitiesComponent::InitializeInstancedAbilities()
{
	int32 SetIdx = 0;
	for (FGASAbilitySetConfig& Set : AbilitiesConfig.Sets)
	{
		FGASActiveAbilitySet AbSet;
		AbSet.InputOverride = nullptr;
		for (int32 Idx = 0; Idx < Set.MaxSlots; Idx++)
		{
			FGASActiveAbility ab;
			ab.SetIndex = SetIdx;
			ab.SlotIndex = Idx;
			if (Set.MaxAbilitiesInSlot > 0)
				ab.ActiveAbilities.SetNum(Set.MaxAbilitiesInSlot);
			else
				ab.ActiveAbilities.SetNum(1);
			AbSet.Abilities.Add(ab);
		}
		ActiveAbilityContainer.AbilitySets.Add(AbSet);
		SetIdx++;
	}
	if (ActiveAbilityContainer.AbilitySets.IsValidIndex(0))
	{
		//ActiveAbilityContainer.AbilitySets[0].InputOverride = NewObject<UGASInputOverride>();
		//ActiveAbilityContainer.AbilitySets[0].InputOverride->AbilityComp = this;
		//ActiveAbilityContainer.AbilitySets[0].InputOverride->SetIndex = 0;
	}
}

void UGASAbilitiesComponent::OnRep_PlayMontage()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(RepMontage.CurrentMontage);
		if (RepMontage.SectionName != NAME_None)
		{
			AnimInst->Montage_JumpToSection(RepMontage.SectionName, RepMontage.CurrentMontage);
		}
		UE_LOG(GameAbilities, Log, TEXT("OnRep_PlayMontage MontageName: %s SectionNAme: %s ForceRep: %s"), *RepMontage.CurrentMontage->GetName(), *RepMontage.SectionName.ToString(), *FString::FormatAsNumber(RepMontage.ForceRep)); 
	}
}
void UGASAbilitiesComponent::PlayMontage(UAnimMontage* MontageIn, FName SectionName)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		//Probabaly want to do something different here for client non authority montage plays.
		//return;
	}
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(MontageIn);
		if (SectionName != NAME_None)
		{
			AnimInst->Montage_JumpToSection(SectionName, MontageIn);
		}
		UE_LOG(GameAbilities, Log, TEXT("PlayMontage MontageName: %s SectionNAme: %s Where: %s"), *MontageIn->GetName(), *SectionName.ToString(), (GetOwnerRole() < ENetRole::ROLE_Authority ? TEXT("Client") : TEXT("Server")));
		RepMontage.SectionName = SectionName;
		RepMontage.CurrentMontage = MontageIn;
		RepMontage.ForceRep++;
	}
}