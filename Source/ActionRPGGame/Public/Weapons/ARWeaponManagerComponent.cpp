// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
#include "ARItemWeapon.h"
#include "ARCharacter.h"
#include "ARPlayerController.h"

#include "DWBPFunctionLibrary.h"
#include "SDraggableWindowWidget.h"

#include "Engine/World.h"

// Sets default values for this component's properties
UARWeaponManagerComponent::UARWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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
	
}


// Called every frame
void UARWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UARWeaponManagerComponent::EquipWeapon(TSoftClassPtr<UGAAbilityBase> WeaponAbility)
{
	if (WeaponAbility.IsNull())
	{
		return;
	}

	TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();
	if (!AbilityComp)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(WeaponAbility));
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponInputRead,
			WeaponAbility, WeaponInput);

		AbilityComp->SetAbilityToAction(WeaponAbility, WeaponInput, ReadyDelegate);
	}
	else
	{
		AbilityComp->SetAbilityToAction(WeaponAbility, WeaponInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(WeaponAbility);
	}
}
UGAAbilityBase* UARWeaponManagerComponent::GetCurrentWeapon()
{
	return GetAbility(ActiveGroup, EAMSlot::Slot001);
}

void UARWeaponManagerComponent::AddToWeaponInventory(TSubclassOf<class UARItemWeapon> InWeapon)
{
}

void UARWeaponManagerComponent::BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
void UARWeaponManagerComponent::AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
}
void UARWeaponManagerComponent::ServerAddWeaponToManager_Implementation(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
bool UARWeaponManagerComponent::ServerAddWeaponToManager_Validate(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	return true;
}



void UARWeaponManagerComponent::OnWeaponInputRead(TSoftClassPtr<UGAAbilityBase> WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
{
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();

	//AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
}


void UARWeaponManagerComponent::OnAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag
	, const TArray<FGameplayTag>& InAbilityInput
	, EAMGroup InGroup, EAMSlot InSlot)
{
	AARPlayerController* PC = Cast<AARPlayerController>(GetOwner());
	if (PC)
	{
		if (AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn()))
		{
			UGAAbilityBase* AbilityInstance = GetAbility(InGroup, InSlot);
			Character->WeaponInventory->SetAbilityToItem(static_cast<uint8>(InGroup), AbilityInstance);
		}
		
	}
}