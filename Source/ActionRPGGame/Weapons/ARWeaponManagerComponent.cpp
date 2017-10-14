// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
// Sets default values for this component's properties
UARWeaponManagerComponent::UARWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	AllWeapons.SetNum(4);
	// ...
}


// Called when the game starts
void UARWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UARWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UARWeaponManagerComponent::BP_EquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex)
{
	NativeEquipWeapon(InAbilityTag, SlotIndex);
}
void UARWeaponManagerComponent::NativeEquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex)
{
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	FAFOnAbilityReady del = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponReady, InAbilityTag, SlotIndex);
	AbilityComp->AddOnAbilityReadyDelegate(InAbilityTag, del);
	AbilityComp->NativeAddAbilityFromTag(InAbilityTag, nullptr);// , /*Input*/ ShootInput);
}
void UARWeaponManagerComponent::OnWeaponReady(FGameplayTag InAbilityTag, int32 SlotIndex)
{
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	UARWeaponAbilityBase* Ability = Cast<UARWeaponAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	if (SlotIndex == 0)
	{
		AllWeapons[SlotIndex] = Ability;
		CurrentWeapon = Ability;
		ActiveWeaponIndex = SlotIndex;
		AbilityComp->SetAbilityToAction(InAbilityTag, ShootInput, FAFOnAbilityReady());
		AbilityComp->SetAbilityToAction(InAbilityTag, ReloadInput, FAFOnAbilityReady());
	}
	else
	{
		AllWeapons[SlotIndex] = Ability;
	}
}

void UARWeaponManagerComponent::NextWeapon()
{
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		ActiveWeaponIndex++;
		if (ActiveWeaponIndex < 4)
		{
			CurrentWeapon = AllWeapons[ActiveWeaponIndex];
		}
		else
		{
			ActiveWeaponIndex = 0;
			CurrentWeapon = AllWeapons[ActiveWeaponIndex];
		}
	}

	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNextWeapon(ActiveWeaponIndex);
	}

}
void UARWeaponManagerComponent::PreviousWeapon()
{
	ActiveWeaponIndex--;
	if (ActiveWeaponIndex >= 0)
	{
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerPreviousWeapon(ActiveWeaponIndex);
	}

}

void UARWeaponManagerComponent::ServerNextWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex++;
	if (ActiveWeaponIndex < 4)
	{
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	//so Server index is different. Client might tried to cheat
	//or sometrhing. We will override it.
	//situation where client can chage multiple weapons within second
	//should not have place, as there is animation and/or internal cooldown on weapon change.
	//since it will be done trough ability.
	if (ActiveWeaponIndex != WeaponIndex)
	{
		ClientNextWeapon(ActiveWeaponIndex);
	}
}
bool UARWeaponManagerComponent::ServerNextWeapon_Validate(int32 WeaponIndex)
{
	return true;
}
void UARWeaponManagerComponent::ClientNextWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex = WeaponIndex;
	CurrentWeapon = AllWeapons[ActiveWeaponIndex];
}
void UARWeaponManagerComponent::ServerPreviousWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex--;
	if (ActiveWeaponIndex >= 0)
	{
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		CurrentWeapon = AllWeapons[ActiveWeaponIndex];
	}
	if (ActiveWeaponIndex != WeaponIndex)
	{

	}
}
bool UARWeaponManagerComponent::ServerPreviousWeapon_Validate(int32 WeaponIndex)
{
	return true;
}