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
void UARWeaponManagerComponent::BP_EquipWeapon(EAMGroup Group, EAMSlot Slot, int32 Idx)
{
	EquipWeapon(Group, Slot, Idx);
}
void UARWeaponManagerComponent::EquipWeapon(EAMGroup Group, EAMSlot Slot, int32 Idx)
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
	}
	Weapons.Add(Weapon);
}
void UARWeaponManagerComponent::NextWeapon()
{
	FGameplayTag CurrentWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	NextGroup();
	FGameplayTag NextWeaponAbility = GetAbilityTag(ActiveGroup, EAMSlot::Slot001);
	if (!NextWeaponAbility.IsValid())
		return;
	TArray<FGameplayTag> WeaponInput = GetInputTag(EAMGroup::Group001, EAMSlot::Slot001);
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(NextWeaponAbility));
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARWeaponManagerComponent::OnWeaponInputRead,
			NextWeaponAbility, WeaponInput);

		AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, ReadyDelegate);
	}
	else
	{
		AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(NextWeaponAbility);
		CurrentWeapon = AbilityToWeapon.FindRef(NextWeaponAbility);
		CurrentWeapon->Equip();
	}
}
void UARWeaponManagerComponent::PreviousWeapon()
{
	PreviousGroup();
}


void UARWeaponManagerComponent::OnWeaponInputRead(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags)
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

	//AbilityComp->SetAbilityToAction(NextWeaponAbility, WeaponInput, FAFOnAbilityReady());
}