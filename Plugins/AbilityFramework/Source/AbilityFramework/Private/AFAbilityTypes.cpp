// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityComponent.h"
#include "Abilities/GAAbilityBase.h"
#include "AFAbilityTypes.h"

void FAFAbilitySpec::PreReplicatedRemove(const struct FAFAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		FAFAbilityContainer& InArraySerializerC = const_cast<FAFAbilityContainer&>(InArraySerializer);
		InArraySerializerC.SpecMap.Remove(Handle);
		//remove attributes
		//UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = nullptr;
	}
}
void FAFAbilitySpec::PostReplicatedAdd(const struct FAFAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		//should be safe, since we only modify the non replicated part of struct.
		FAFAbilityContainer& InArraySerializerC = const_cast<FAFAbilityContainer&>(InArraySerializer);
		Ability->AbilityComponent = InArraySerializer.AbilitiesComp.Get();
		if (InArraySerializer.AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(InArraySerializer.AbilitiesComp->GetOwner());
			Ability->POwner = POwner;
			Ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			Ability->OwnerCamera = nullptr;
		}
		Ability->InitAbility();
		Ability->Attributes = nullptr;

		//TODO - CHANGE ATTRIBUTE HANDLING
		UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = attr;
		InArraySerializerC.SpecMap.Add(Handle, *this);
		InArraySerializerC.AbilitiesComp->NotifyOnAbilityReady(*this, Handle, ClientHandle);
	}
}
void FAFAbilitySpec::PostReplicatedChange(const struct FAFAbilityContainer& InArraySerializer)
{

}

UGAAbilityBase* FAFAbilityContainer::AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn
	, const FAFAbilitySpecHandle Handle, const FAFAbilitySpecHandle ClientHandle)
{
	ENetMode NetMode = AbilitiesComp->GetNetMode();
	if (AbilityIn && AbilitiesComp.IsValid())
	{
		UGAAbilityBase* ability = NewObject<UGAAbilityBase>(AbilitiesComp->GetOwner(), AbilityIn);
		ability->AbilityComponent = AbilitiesComp.Get();
		if (AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(AbilitiesComp->GetOwner());
			ability->POwner = POwner;
			ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			ability->OwnerCamera = nullptr;
		}
		ability->InitAbility();
		FGameplayTag Tag = ability->AbilityTag;
		FAFAbilitySpec Spec;
		Spec.Ability = ability;
		Spec.Handle = Handle;
		Spec.ClientHandle = ClientHandle;
		MarkItemDirty(Spec);
		ActivatableAbilities.Add(Spec);
		SpecMap.Add(Handle, Spec);

		MarkArrayDirty();
		if (AbilitiesComp->GetNetMode() == ENetMode::NM_Standalone
			|| AbilitiesComp->GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			//AbilitiesComp->NotifyOnAbilityReady(InClassPtr);
		}

		return ability;
	}
	return nullptr;
}

void FAFAbilityContainer::RemoveAbility(const FAFAbilitySpecHandle AbilityIn)
{
	auto pred = [AbilityIn](const FAFAbilitySpec& Item) -> bool
	{
		if (Item.Handle == AbilityIn)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	int32 Index = ActivatableAbilities.IndexOfByPredicate(pred);

	if (Index == INDEX_NONE)
		return;

	UGAAbilityBase* Ability = SpecMap.FindRef(AbilityIn).Ability;
	

	for (auto It = Ability->AbilityTasks.CreateIterator(); It; ++It)
	{
		AbilitiesComp->ReplicatedTasks.Remove(It->Value);
	}
	Ability->AbilityTasks.Reset();

	SpecMap.Remove(AbilityIn);
	MarkItemDirty(ActivatableAbilities[Index]);
	ActivatableAbilities.RemoveAt(Index);
	MarkArrayDirty();
}


UGAAbilityBase* FAFAbilityContainer::GetAbility(FAFAbilitySpecHandle InAbiltyPtr)
{
	UGAAbilityBase* retAbility = SpecMap.FindRef(InAbiltyPtr).Ability;
	return retAbility;
}

void FAFAbilityContainer::HandleInputPressed(const uint8 InputID, const FAFPredictionHandle& InPredictionHandle)
{
	FAFAbilitySpec Spec = InputToAbilitySpec.FindRef(InputID);
	

	ENetMode NetMode = AbilitiesComp->GetNetMode();
	if(Spec.Ability)
		Spec.Ability->OnNativeInputPressed(InputID, InPredictionHandle);
}
void FAFAbilityContainer::HandleInputReleased(const uint8 InputID)
{
	FAFAbilitySpec Spec = InputToAbilitySpec.FindRef(InputID);
	
	ENetMode NetMode = AbilitiesComp->GetNetMode();
	if(Spec.Ability)
		Spec.Ability->OnNativeInputReleased(InputID);
}

void FAFAbilityContainer::BindAbilityToInputIDs(const FAFAbilitySpecHandle Handle, TArray<uint8> InputIDs)
{
	ENetMode NetMode = AbilitiesComp->GetNetMode();

	FAFAbilitySpec Spec = SpecMap[Handle];

	for (uint8 ID : InputIDs)
	{
		FAFAbilitySpec& Ref = InputToAbilitySpec.FindOrAdd(ID);
		Ref = Spec;
	}
	Spec.Ability->OnAbilityInputReady();
}
