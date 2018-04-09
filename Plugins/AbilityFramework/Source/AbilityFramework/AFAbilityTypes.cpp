// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityComponent.h"
#include "Abilities/GAAbilityBase.h"
#include "AFAbilityTypes.h"

void FAFAbilityItem::PreReplicatedRemove(const struct FAFAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		FAFAbilityContainer& InArraySerializerC = const_cast<FAFAbilityContainer&>(InArraySerializer);
		//remove attributes
		//UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = nullptr;
		InArraySerializerC.AbilityToAction.Remove(AbilityClass);
		InArraySerializerC.AbilitiesInputs.Remove(AbilityClass);
		InArraySerializerC.TagToAbility.Remove(AbilityClass);
	}
}
void FAFAbilityItem::PostReplicatedAdd(const struct FAFAbilityContainer& InArraySerializer)
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

		InArraySerializerC.AbilitiesInputs.Add(AbilityClass, Ability); //.Add(Ability->AbilityTag, Ability);
		InArraySerializerC.TagToAbility.Add(AbilityClass, Ability);
		InArraySerializerC.AbilitiesComp->NotifyOnAbilityReady(AbilityClass);
	}
}
void FAFAbilityItem::PostReplicatedChange(const struct FAFAbilityContainer& InArraySerializer)
{

}


void FAFAbilityContainer::SetBlockedInput(const FGameplayTag& InActionName, bool bBlock)
{
	if (BlockedInput.Contains(InActionName))
	{
		BlockedInput[InActionName] = bBlock;
	}
	else
	{
		BlockedInput.Add(InActionName, bBlock);
	}
}
UGAAbilityBase* FAFAbilityContainer::AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn
	, TSoftClassPtr<class UGAAbilityBase> InClassPtr)
{
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

		AbilitiesInputs.Add(InClassPtr, ability);
		FAFAbilityItem AbilityItem(ability, InClassPtr);
		MarkItemDirty(AbilityItem);
		AbilityItem.Ability = ability;
		AbilitiesItems.Add(AbilityItem);
		TagToAbility.Add(InClassPtr, ability);

		MarkArrayDirty();
		if (AbilitiesComp->GetNetMode() == ENetMode::NM_Standalone
			|| AbilitiesComp->GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			AbilitiesComp->NotifyOnAbilityReady(InClassPtr);
		}
		/*	if (ActionName.IsValid())
		{
		UInputComponent* InputComponent = AbilitiesComp->GetOwner()->FindComponentByClass<UInputComponent>();
		AbilitiesComp->BindAbilityToAction(InputComponent, ActionName, Tag);
		}*/
		return ability;
	}
	return nullptr;
}
void FAFAbilityContainer::RemoveAbility(const TSoftClassPtr<UGAAbilityBase>& AbilityIn)
{
	int32 Index = AbilitiesItems.IndexOfByKey(AbilityIn);

	if (Index == INDEX_NONE)
		return;

	UGAAbilityBase* Ability = TagToAbility.FindRef(AbilityIn);

	for (auto It = Ability->AbilityTasks.CreateIterator(); It; ++It)
	{
		AbilitiesComp->ReplicatedTasks.Remove(It->Value);
	}

	TagToAbility.Remove(AbilityIn);
	MarkItemDirty(AbilitiesItems[Index]);
	AbilitiesItems.RemoveAt(Index);
	MarkArrayDirty();
}
TSoftClassPtr<UGAAbilityBase> FAFAbilityContainer::IsAbilityBoundToAction(const FGameplayTag& InInputTag)
{
	TSoftClassPtr<UGAAbilityBase> Ability;
	TSoftClassPtr<UGAAbilityBase>* AbilityTag = ActionToAbility.Find(InInputTag);
	if (AbilityTag)
	{
		Ability = *AbilityTag;
	}

	return Ability;
}

void FAFAbilityContainer::SetAbilityToAction(const TSoftClassPtr<UGAAbilityBase>& InAbiltyPtr, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& InputTag : InInputTag)
	{
		TSoftClassPtr<UGAAbilityBase>& AbilityClassPtr = ActionToAbility.FindOrAdd(InputTag);
		AbilityClassPtr = InAbiltyPtr;
		TArray<FGameplayTag>& ActionTag = AbilityToAction.FindOrAdd(InAbiltyPtr);
		ActionTag.Add(InputTag);
	}
	if (!AbilitiesComp.IsValid())
		return;

	UGAAbilityBase* Ability = TagToAbility.FindRef(InAbiltyPtr);
	if (Ability)
	{
		Ability->OnAbilityInputReady();
	}

	if (AbilitiesComp->GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		AbilitiesComp->ClientNotifyAbilityInputReady(InAbiltyPtr);
	}
}

UGAAbilityBase* FAFAbilityContainer::GetAbility(TSoftClassPtr<UGAAbilityBase> InAbiltyPtr)
{
	UGAAbilityBase* retAbility = AbilitiesInputs.FindRef(InAbiltyPtr);
	return retAbility;
}
void FAFAbilityContainer::HandleInputPressed(FGameplayTag ActionName, const FAFPredictionHandle& InPredictionHandle)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	TSoftClassPtr<UGAAbilityBase> AbiltyPtr = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(AbiltyPtr);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		ability->OnNativeInputPressed(ActionName, InPredictionHandle);
	}
}
void FAFAbilityContainer::HandleInputReleased(FGameplayTag ActionName)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	TSoftClassPtr<UGAAbilityBase> abilityTag = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(abilityTag);
	if (ability)
	{
		ability->OnNativeInputReleased(ActionName);
	}
}

void FAFAbilityContainer::TriggerAbylityByTag(TSoftClassPtr<UGAAbilityBase> InTag)
{
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(InTag);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		FAFPredictionHandle PredHandle = FAFPredictionHandle::GenerateClientHandle(AbilitiesComp.Get());
		ability->OnNativeInputPressed(FGameplayTag(), PredHandle);
	}
}