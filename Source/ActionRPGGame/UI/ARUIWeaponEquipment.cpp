// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIWeaponEquipment.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
// Sets default values for this component's properties
UARUIWeaponEquipment::UARUIWeaponEquipment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Weapons.SetNum(4);
	// ...
}


// Called when the game starts
void UARUIWeaponEquipment::BeginPlay()
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
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	AbilityComp->BindAbilityToAction(InputComponent, ShootInput);
	AbilityComp->BindAbilityToAction(InputComponent, ReloadInput);
	// ...
	
}


// Called every frame
void UARUIWeaponEquipment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UARUIWeaponEquipment::NativeEquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex)
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

	FAFOnAbilityReady del = FAFOnAbilityReady::CreateUObject(this, &UARUIWeaponEquipment::OnWeaponReady, InAbilityTag, SlotIndex);
	AbilityComp->AddOnAbilityReadyDelegate(InAbilityTag, del);
	AbilityComp->NativeAddAbilityFromTag(InAbilityTag, nullptr, /*Input*/ ShootInput);
}

void UARUIWeaponEquipment::OnWeaponReady(FGameplayTag InAbilityTag, int32 SlotIndex)
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

	UARAbilityBase* Ability = Cast<UARAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	if (SlotIndex == 0)
	{
		Weapons[SlotIndex] = Ability;
		ActiveWeapon = Ability;
		ActiveWeaponIndex = SlotIndex;
		AbilityComp->SetAbilityToAction(InAbilityTag, ShootInput);
		AbilityComp->SetAbilityToAction(InAbilityTag, ReloadInput);
	}
	else
	{
		Weapons[SlotIndex] = Ability;
	}
}
void UARUIWeaponEquipment::BP_EquipWeapon(const FGameplayTag& InAbilityTag, int32 SlotIndex)
{
	NativeEquipWeapon(InAbilityTag, SlotIndex);
}

void UARUIWeaponEquipment::NextWeapon()
{
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		ActiveWeaponIndex++;
		if (ActiveWeaponIndex < 4)
		{
			ActiveWeapon = Weapons[ActiveWeaponIndex];
		}
		else
		{
			ActiveWeaponIndex = 0;
			ActiveWeapon = Weapons[ActiveWeaponIndex];
		}
		ServerNextWeapon(ActiveWeaponIndex);
	}

}
void UARUIWeaponEquipment::PreviousWeapon()
{
	ActiveWeaponIndex--;
	if (ActiveWeaponIndex >= 0)
	{
		ActiveWeapon = Weapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		ActiveWeapon = Weapons[ActiveWeaponIndex];
	}
	ServerPreviousWeapon(ActiveWeaponIndex);
}

void UARUIWeaponEquipment::ServerNextWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex++;
	if (ActiveWeaponIndex < 4)
	{
		ActiveWeapon = Weapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		ActiveWeapon = Weapons[ActiveWeaponIndex];
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
bool UARUIWeaponEquipment::ServerNextWeapon_Validate(int32 WeaponIndex)
{
	return true;
}
void UARUIWeaponEquipment::ClientNextWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex = WeaponIndex;
	ActiveWeapon = Weapons[ActiveWeaponIndex];
}
void UARUIWeaponEquipment::ServerPreviousWeapon_Implementation(int32 WeaponIndex)
{
	ActiveWeaponIndex--;
	if (ActiveWeaponIndex >= 0)
	{
		ActiveWeapon = Weapons[ActiveWeaponIndex];
	}
	else
	{
		ActiveWeaponIndex = 0;
		ActiveWeapon = Weapons[ActiveWeaponIndex];
	}
	if (ActiveWeaponIndex != WeaponIndex)
	{

	}
}
bool UARUIWeaponEquipment::ServerPreviousWeapon_Validate(int32 WeaponIndex)
{
	return true;
}