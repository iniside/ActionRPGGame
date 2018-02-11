// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
#include "ARItemWeapon.h"
#include "../ARCharacter.h"
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
	EquipedWeapons.SetNum(MAX_WEAPONS+1);
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;
	
	//POwner = MyPC->GetPawn();
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

void UARWeaponManagerComponent::AddToWeaponInventory(TSubclassOf<class UARItemWeapon> InWeapon)
{
	WeaponClasses.Add(InWeapon);
}

void UARWeaponManagerComponent::BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
void UARWeaponManagerComponent::AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	if (Character)
	{
		Character->GetWeapons()->Holster(Group, WeaponClasses[Idx].GetDefaultObject());
		EquipedWeapons[AMEnumToInt<EAMGroup>(Group)] = WeaponClasses[Idx].GetDefaultObject();
		ActiveGroup = EAMGroup::Group005;
	}
	NativeEquipAbility(WeaponClasses[Idx].GetDefaultObject()->Ability,
		Group, EAMSlot::Slot001, POwner);

	if (IsClient())
	{
		ServerAddWeaponToManager(Group, Slot, Idx);
	}
	else
	{
		APlayerController* MyPC = Cast<APlayerController>(GetOwner());
		if (!MyPC)
			return;
		
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(POwner);
		if (!ABInt)
			return;

		Character->GetWeapons()->Holster(Group, WeaponClasses[Idx].GetDefaultObject());
		EquipedWeapons[AMEnumToInt<EAMGroup>(Group)] = WeaponClasses[Idx].GetDefaultObject();
		ActiveGroup = EAMGroup::Group005;
	}

}
void UARWeaponManagerComponent::ServerAddWeaponToManager_Implementation(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
bool UARWeaponManagerComponent::ServerAddWeaponToManager_Validate(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	return true;
}

void UARWeaponManagerComponent::NextWeapon()
{
	EAMGroup OldGroup = ActiveGroup;
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
		CurrentIndex++;
		if (CurrentIndex > MAX_WEAPONS)
		{
			ActiveGroup = EAMGroup::Group001;
		}
		else
		{
			ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
		}

	}

	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindNextValid();
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNextWeapon(AMEnumToInt<EAMGroup>(ActiveGroup));
	}

	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility, OldGroup);
}
void UARWeaponManagerComponent::PreviousWeapon()
{
	EAMGroup OldGroup = ActiveGroup;
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	CurrentIndex--;
	ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	if (CurrentIndex < 0)
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(Groups.Num() - 1);
	}
	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindPreviousValid();
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerPreviousWeapon(static_cast<int32>(ActiveGroup));
	}
	
	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility, OldGroup);
}

void UARWeaponManagerComponent::HolsterWeapon()
{
	ActiveGroup = EAMGroup::Group005;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		Character->GetWeapons()->HolsterActive(ActiveGroup);
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerHolsterWeapon(static_cast<int32>(ActiveGroup));
	}
}

void UARWeaponManagerComponent::ServerNextWeapon_Implementation(int32 WeaponIndex)
{
	EAMGroup OldGroup = ActiveGroup;
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);

	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	if(WeaponIndex > CurrentIndex)
		CurrentIndex++;

	if (CurrentIndex > MAX_WEAPONS)
	{
		ActiveGroup = EAMGroup::Group001;
	}
	else
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	}

	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindNextValid();
	}
	if (WeaponIndex == AMEnumToInt<EAMGroup>(ActiveGroup))
	{
		ClientNextWeapon(AMEnumToInt<EAMGroup>(ActiveGroup), true);
	}
	else
	{
		ClientNextWeapon(AMEnumToInt<EAMGroup>(ActiveGroup), false);
	}

	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility, OldGroup);
}
bool UARWeaponManagerComponent::ServerNextWeapon_Validate(int32 WeaponIndex)
{
	return true;
}
void UARWeaponManagerComponent::ClientNextWeapon_Implementation(int32 WeaponIndex, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;
}

void UARWeaponManagerComponent::ServerPreviousWeapon_Implementation(int32 WeaponIndex)
{
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	if(CurrentIndex > WeaponIndex)
		CurrentIndex--;

	if (CurrentIndex < 0)
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(MAX_WEAPONS - 1);
	}
	else
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	}

	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindPreviousValid();
	}
	//so Server index is different. Client might tried to cheat
	//or sometrhing. We will override it.
	//situation where client can chage multiple weapons within second
	//should not have place, as there is animation and/or internal cooldown on weapon change.
	//since it will be done trough ability.
	if (ActiveGroup != AMIntToEnum<EAMGroup>(WeaponIndex))
	{
		ClientNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup), false);
	}
	else
	{
		ClientNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup), true);
	}
}
bool UARWeaponManagerComponent::ServerPreviousWeapon_Validate(int32 WeaponIndex)
{
	return true;
}
void UARWeaponManagerComponent::ClientPreviousWeapon_Implementation(int32 WeaponIndex, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;
}

void UARWeaponManagerComponent::ServerHolsterWeapon_Implementation(int32 WeaponIndex)
{
	ActiveGroup = EAMGroup::Group005;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		Character->GetWeapons()->HolsterActive(ActiveGroup);
	}
}
bool UARWeaponManagerComponent::ServerHolsterWeapon_Validate(int32 WeaponIndex)
{
	return true;
}

void UARWeaponManagerComponent::OnWeaponInputRead(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
{
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();

	//AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
}

void UARWeaponManagerComponent::EquipWeapon(const FGameplayTag& PreviousWeaponTag, const FGameplayTag& NextWeaponTag, EAMGroup OldGroup)
{
	if (!NextWeaponTag.IsValid())
	{
		return;
	}
	TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();
	if (!AbilityComp)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(NextWeaponTag));
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	if (Character)
	{
		Character->GetWeapons()->EquipInactive(ActiveGroup, EquipedWeapons[AMEnumToInt<EAMGroup>(ActiveGroup)]	
			, OldGroup
			, EquipedWeapons[AMEnumToInt<EAMGroup>(OldGroup)]);
	}
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponInputRead,
			NextWeaponTag, WeaponInput);

		AbilityComp->SetAbilityToAction(NextWeaponTag, WeaponInput, ReadyDelegate);
	}
	else
	{
		AbilityComp->SetAbilityToAction(NextWeaponTag, WeaponInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(NextWeaponTag);
	}
}
FGameplayTag UARWeaponManagerComponent::FindNextValid()
{
	FGameplayTag WeaponAbilityTag;

	int32 Idx = AMEnumToInt<EAMGroup>(ActiveGroup);
	while (!WeaponAbilityTag.IsValid())
	{
		Idx++;
		if (Idx > MAX_WEAPONS - 1)
		{
			Idx = 0;
		}
		WeaponAbilityTag = GetAbilityTag(AMIntToEnum<EAMGroup>(Idx), EAMSlot::Slot001);

		//no weapon at first index, just assume there is nothing equipped.
		if ((Idx == 0) && !WeaponAbilityTag.IsValid())
		{
			SelectGroup(EAMGroup::Group005);
			break;
		}
	}
	if (WeaponAbilityTag.IsValid())
	{
		SelectGroup(AMIntToEnum<EAMGroup>(Idx));
	}
	return WeaponAbilityTag;
}

FGameplayTag UARWeaponManagerComponent::FindPreviousValid()
{
	FGameplayTag WeaponAbilityTag;
	int32 Idx = AMEnumToInt<EAMGroup>(ActiveGroup);
	while (!WeaponAbilityTag.IsValid())
	{
		Idx--;
		if (Idx < 0)
		{
			Idx = MAX_WEAPONS - 1;
		}

		//again no ability for weapon.
		if ((Idx == (MAX_WEAPONS - 1) ) && !WeaponAbilityTag.IsValid())
		{
			SelectGroup(EAMGroup::Group005);
			break;
		}
		WeaponAbilityTag = GetAbilityTag(AMIntToEnum<EAMGroup>(Idx), EAMSlot::Slot001);
	}
	if (WeaponAbilityTag.IsValid())
	{
		SelectGroup(AMIntToEnum<EAMGroup>(Idx));
	}
	return WeaponAbilityTag;
}

//void UARWeaponManagerComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(UARWeaponManagerComponent, EquippedWeapons);
//}

bool UARWeaponManagerComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	return WroteSomething;
}
void UARWeaponManagerComponent::OnAbilityReady(const FGameplayTag& InAbilityTag
	, const TArray<FGameplayTag>& InAbilityInput
	, EAMGroup InGroup, EAMSlot InSlot)
{
	/*UAFAbilityComponent* AbilityComponent = GetAbilityComponent();
	if (!AbilityComponent)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComponent->BP_GetAbilityByTag(InAbilityTag));
	SetAbility(InGroup, EAMSlot::Slot001, Ability);
	SetAbilityTag(InGroup, EAMSlot::Slot001, InAbilityTag);*/
}
