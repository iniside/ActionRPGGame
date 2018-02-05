// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerComponent.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARWeaponAbilityBase.h"
#include "ARWeaponBase.h"
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
UGAAbilityBase* UARWeaponManagerComponent::GetCurrentWeapon()
{
	return GetAbility(ActiveGroup, EAMSlot::Slot001);
}

void UARWeaponManagerComponent::AddToWeaponInventory(TSubclassOf<class AARWeaponBase> InWeapon)
{
	WeaponClasses.Add(InWeapon);
}

void UARWeaponManagerComponent::BP_AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	AddWeaponToManager(Group, Slot, Idx);
}
void UARWeaponManagerComponent::AddWeaponToManager(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	if (IsClient())
	{
		ServerAddWeaponToManager(Group, Slot, Idx);
		if (Group == EAMGroup::Group001)
		{
			UAFAbilityComponent* AbilityComp = GetAbilityComponent();
			FAFOnAbilityReady ReadyDel = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::ClientOnWeaponAbilityReady,
				WeaponClasses[Idx].GetDefaultObject()->GetWeaponAbility(), Group);

			AbilityComp->AddOnAbilityReadyDelegate(WeaponClasses[Idx].GetDefaultObject()->GetWeaponAbility(), ReadyDel);

			TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
			//if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
			{
				FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponInputRead,
					WeaponClasses[Idx].GetDefaultObject()->GetWeaponAbility(), WeaponInput);

				AbilityComp->SetAbilityToAction(WeaponClasses[Idx].GetDefaultObject()->GetWeaponAbility(), WeaponInput, ReadyDelegate);
			}
		}
	}
	else
	{
		if (WeaponClasses.Num() < 0)
			return;
		APlayerController* MyPC = Cast<APlayerController>(GetOwner());
		if (!MyPC)
			return;
		APawn* POwner = MyPC->GetPawn();
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(POwner);
		if (!ABInt)
			return;

		UWorld* World = GetWorld();

		FActorSpawnParameters SpawnParams;
		AARWeaponBase* Weapon = World->SpawnActor<AARWeaponBase>(WeaponClasses[Idx], SpawnParams);
		Weapon->OnAddToWeaponManager(Group, Slot, POwner, this);
		AbilityToWeapon.Add(Weapon->GetWeaponAbility(), Weapon);
		if (Group == EAMGroup::Group001)
		{
			CurrentWeapon = Weapon;
			CurrentWeapon->Equip();
			UAFAbilityComponent* AbilityComp = GetAbilityComponent();
			TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
			
			{
				AbilityComp->SetAbilityToAction(Weapon->GetWeaponAbility(), WeaponInput, FAFOnAbilityReady());
				ExecuteAbilityReadyEvent(Weapon->GetWeaponAbility());
			}
		}
		Weapons.Add(Weapon);
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
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	NextGroup();
	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindNextValid();
	}
	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility);
}
void UARWeaponManagerComponent::PreviousWeapon()
{
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	PreviousGroup();
	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
	{
		NextWeaponAbility = FindPreviousValid();
	}
	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility);
}


void UARWeaponManagerComponent::OnWeaponInputRead(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
{
	UAFAbilityComponent* AbilityComp = GetAbilityComponent();

	//AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
}
void UARWeaponManagerComponent::OnNextGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;

	//something gone wrong.. we need to change weapon to correct one.
	//we probabaly already changed. So just get current group.
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);

	//and then get correct one from server.
	FGameplayTag NextWeaponAbility = GetAbilityTag(ValidGroup, EAMSlot::Slot001);
	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility);
}
void UARWeaponManagerComponent::OnPreviousGroupConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;

	//something gone wrong.. we need to change weapon to correct one.
	//we probabaly already changed. So just get current group.
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);

	//and then get correct one from server.
	FGameplayTag NextWeaponAbility = GetAbilityTag(ValidGroup, EAMSlot::Slot001);
	EquipWeapon(CurrentWeaponAbility, NextWeaponAbility);
}

void UARWeaponManagerComponent::EquipWeapon(const FGameplayTag& PreviousWeaponTag, const FGameplayTag& NextWeaponTag)
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
		AARWeaponBase* PreviousWeapon = AbilityToWeapon.FindRef(PreviousWeaponTag);
		PreviousWeapon->UnEquip();
		CurrentWeapon = AbilityToWeapon.FindRef(NextWeaponTag);
		CurrentWeapon->Equip();
	}
}
FGameplayTag UARWeaponManagerComponent::FindNextValid()
{
	FGameplayTag WeaponAbilityTag;

	int32 Idx = AMEnumToInt<EAMGroup>(ActiveGroup);
	while (!WeaponAbilityTag.IsValid())
	{
		Idx++;
		if (Idx > Groups.Num() - 1)
		{
			Idx = 0;
		}
		WeaponAbilityTag = GetAbilityTag(AMIntToEnum<EAMGroup>(Idx), EAMSlot::Slot001);
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
			Idx = 0;
		}
		WeaponAbilityTag = GetAbilityTag(AMIntToEnum<EAMGroup>(Idx), EAMSlot::Slot001);
	}
	if (WeaponAbilityTag.IsValid())
	{
		SelectGroup(AMIntToEnum<EAMGroup>(Idx));
	}
	return WeaponAbilityTag;
}

void UARWeaponManagerComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UARWeaponManagerComponent, CurrentWeapon);
}

void UARWeaponManagerComponent::OnRep_CurrentWeapon()
{
	if (CurrentWeapon)
	{
		int sd = 0;
	}
}
void UARWeaponManagerComponent::OnWeaponAbilityReady(const FGameplayTag& WeaponAbility, EAMGroup InGroup)
{
	UAFAbilityComponent* AbilityComponent = GetAbilityComponent();
	if (!AbilityComponent)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComponent->BP_GetAbilityByTag(WeaponAbility));
	SetAbility(InGroup, EAMSlot::Slot001, Ability);
	SetAbilityTag(InGroup, EAMSlot::Slot001, WeaponAbility);
}
void UARWeaponManagerComponent::ClientOnWeaponAbilityReady(FGameplayTag WeaponAbility, EAMGroup InGroup)
{
	UAFAbilityComponent* AbilityComponent = GetAbilityComponent();
	if (!AbilityComponent)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComponent->BP_GetAbilityByTag(WeaponAbility));
	SetAbility(InGroup, EAMSlot::Slot001, Ability);
	SetAbilityTag(InGroup, EAMSlot::Slot001, WeaponAbility);
}