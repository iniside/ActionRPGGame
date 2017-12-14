// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
#include "../Weapons/ARWeaponManagerComponent.h"
// Sets default values for this component's properties
UARWeaponManagerComponent::UARWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	AbilitySet.SetNum(MaxGroups);
	if (MaxGroups > 0)
	{
		AbilitySet[0].SetNum(1);
		AbilitySet[1].SetNum(1);
		AbilitySet[2].SetNum(1);
		AbilitySet[3].SetNum(1);
	}
	AbilityTagsSet.SetNum(MaxGroups);
	if (MaxGroups > 0)
	{
		AbilityTagsSet[0].SetNum(1);
		AbilityTagsSet[1].SetNum(1);
		AbilityTagsSet[2].SetNum(1);
		AbilityTagsSet[3].SetNum(1);
	}
	// ...
}


// Called when the game starts
void UARWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	//for (const FGameplayTag& Tag : AbilityInputs)
	{
		AbilityComp->BP_BindAbilityToAction(InputSetup.GetInputs(EAMGroup::Group001, EAMSlot::Slot001)[0]);
		AbilityComp->BP_BindAbilityToAction(InputSetup.GetInputs(EAMGroup::Group001, EAMSlot::Slot001)[1]);
	}
	// ...
	
}


// Called every frame
void UARWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
UGAAbilityBase* UARWeaponManagerComponent::GetCurrentWeapon()
{
	return GetAbility(ActiveGroup, EAMSlot::Slot001);
}

void UARWeaponManagerComponent::NextWeapon()
{
	NextGroup();
}
void UARWeaponManagerComponent::PreviousWeapon()
{
	PreviousGroup();
}