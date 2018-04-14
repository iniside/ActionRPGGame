// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponInventoryComponent.h"
#include "Engine/AssetManager.h"
#include "ARItemWeapon.h"
#include "ARCharacter.h"
#include "ARPlayerController.h"
#include "Weapons/ARWeaponManagerComponent.h"

// Sets default values for this component's properties
UARWeaponInventoryComponent::UARWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WeaponHelper.Add(&Group001HolsteredAttachment);
	WeaponHelper.Add(&Group002HolsteredAttachment);
	WeaponHelper.Add(&Group003HolsteredAttachment);
	WeaponHelper.Add(&Group004HolsteredAttachment);

	MaxSlots = 4;
	AvailableSlots = 4;

	// ...
}


// Called when the game starts
void UARWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		GroupToComponent.Add(0, Character->GetHolsteredRightWeapon());
		GroupToComponent.Add(1, Character->GetHolsteredLeftWeapon());
		GroupToComponent.Add(2, Character->GetHolsteredBackDownWeapon());
		GroupToComponent.Add(3, Character->GetHolsteredSideLeftWeapon());
	}
}


// Called every frame
void UARWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UARWeaponInventoryComponent::SetWeapon(const FARWeapon& InWeapon, UChildActorComponent* Component)
{
	if (InWeapon.Weapon.IsValid() || InWeapon.Weapon.IsNull())
	{
		Component->SetChildActorClass(InWeapon.Weapon.LoadSynchronous());
		Component->SetRelativeLocation(FVector(0, 0, 0));
		Component->SetRelativeRotation(FRotator(0, 0, 0));

		Component->SetRelativeLocation(InWeapon.Position);
		Component->SetRelativeRotation(InWeapon.Rotation);
	}
	else
	{
		FStreamableDelegate LoadFinished = FStreamableDelegate::CreateUObject(this, &UARWeaponInventoryComponent::AsynWeaponLoaded, Component, InWeapon);
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(InWeapon.Weapon.ToSoftObjectPath(), LoadFinished);
	}
}

void UARWeaponInventoryComponent::OnItemAdded(UIFItemBase* Item, uint8 LocalIndex)
{
	UARItemWeapon* InWeapon = Cast<UARItemWeapon>(Item);
	//if (InOldWeapon)
	//{
	//	WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Weapon = MainHandWeapon.Weapon;
	//	//WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Position = InOldWeapon->HolsteredPosition;
	//	//WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Rotation = InOldWeapon->HolsteredRotation;
	//	WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->RepCounter++;
	//	//GroupToComponent[OldGroup]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Weapon.Get());
	//	SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)], GroupToComponent[OldGroup]);
	//}

	MainHandWeapon.Weapon = InWeapon->Weapon;
	MainHandWeapon.Position = InWeapon->EquipedPosition;
	MainHandWeapon.Rotation = InWeapon->EquipedRotation;
	MainHandWeapon.NetIndex = Inventory.LocalToNet[LocalIndex];
	MainHandWeapon.RepCounter++;

	WeaponHelper[LocalIndex]->Weapon.Reset();
	WeaponHelper[LocalIndex]->Weapon = InWeapon->Weapon;
	WeaponHelper[LocalIndex]->RepCounter++;
	//GroupToComponent[Group]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Get());
	SetWeapon(*WeaponHelper[LocalIndex], GroupToComponent[LocalIndex]);
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			FSoftObjectPath d = InWeapon->Ability.ToSoftObjectPath();
			TSoftClassPtr<UGAAbilityBase> dd(d);
			PC->WeaponManager->NativeEquipAbility(dd, AMIntToEnum<EAMGroup>(LocalIndex), EAMSlot::Slot001);
			
		}
	}
	//if (!InOldWeapon)
	//{
	//	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Reset();
	//	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
	//	//GroupToComponent[Group]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Get());
	//	SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(Group)], GroupToComponent[Group]);
	//}
	//if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	//{

	//	SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
	//	//GroupToComponent[Group]->SetChildActorClass(nullptr);
	//}
}

void UARWeaponInventoryComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UARWeaponInventoryComponent, Group001HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponInventoryComponent, Group002HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponInventoryComponent, Group003HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponInventoryComponent, Group004HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponInventoryComponent, MainHandWeapon, COND_SkipOwner);
}

void UARWeaponInventoryComponent::Equip(uint8 WeaponIndex, class UARItemWeapon* InWeapon)
{
	MainHandWeapon.Weapon = InWeapon->Weapon;
	MainHandWeapon.Position = InWeapon->EquipedPosition;
	MainHandWeapon.Rotation = InWeapon->EquipedRotation;
	MainHandWeapon.NetIndex = Inventory.LocalToNet[WeaponIndex];
	MainHandWeapon.RepCounter++;
	WeaponHelper[WeaponIndex]->Weapon.Reset();
	WeaponHelper[WeaponIndex]->RepCounter++;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
		GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			FSoftObjectPath Path = InWeapon->Ability.ToSoftObjectPath();
			TSoftClassPtr<UGAAbilityBase> ab(Path);
			PC->WeaponManager->EquipWeapon(ab);
		}
	}
}

void UARWeaponInventoryComponent::Holster(EAMGroup Group, class UARItemWeapon* InWeapon)
{

}
void UARWeaponInventoryComponent::SetAbilityToItem(uint8 InLocalIndex, class UGAAbilityBase* InAbility)
{

}
void UARWeaponInventoryComponent::NextWeapon()
{
	uint8 OldGroup = CurrentWeaponIndex;
	
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		uint8 CurrentIndex = CurrentWeaponIndex;
		CurrentIndex++;
		if (CurrentIndex > 4)
		{
			CurrentWeaponIndex = 0;
		}
		else
		{
			CurrentWeaponIndex = CurrentIndex;
		}
	}

	UARItemWeapon* NextWeaponAbility = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!NextWeaponAbility)
	{
		NextWeaponAbility = FindNextValid();
	}
	Equip(CurrentWeaponIndex, NextWeaponAbility);
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNextWeapon(CurrentWeaponIndex);
	}
}
void UARWeaponInventoryComponent::PreviousWeapon()
{
	uint8 OldGroup = CurrentWeaponIndex;

	uint8 CurrentIndex = CurrentWeaponIndex;
	CurrentIndex--;
	CurrentWeaponIndex = CurrentIndex;
	if (CurrentIndex < 0)
	{
		CurrentWeaponIndex = 3;
	}
	UARItemWeapon* NextWeaponAbility = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!NextWeaponAbility)
	{
		NextWeaponAbility = FindPreviousValid();
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerPreviousWeapon(CurrentWeaponIndex);
	}
}

void UARWeaponInventoryComponent::HolsterWeapon()
{
	//ActiveGroup = EAMGroup::Group005;
	//if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	//{
	//	Character->GetWeapons()->HolsterActive(ActiveGroup);
	//}
	//if (GetOwnerRole() < ENetRole::ROLE_Authority)
	//{
	//	ServerHolsterWeapon(static_cast<uint8>(ActiveGroup));
	//}
}
void UARWeaponInventoryComponent::ServerHolsterWeapon_Implementation(uint8 WeaponIndex)
{
	//ActiveGroup = EAMGroup::Group005;
	//if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	//{
	//	Character->GetWeapons()->HolsterActive(ActiveGroup);
	//}
}
bool UARWeaponInventoryComponent::ServerHolsterWeapon_Validate(uint8 WeaponIndex)
{
	return true;
}
void UARWeaponInventoryComponent::ServerNextWeapon_Implementation(uint8 WeaponIndex)
{
	uint8 OldGroup = CurrentWeaponIndex;

	uint8 CurrentIndex = CurrentWeaponIndex;
	//if (WeaponIndex > CurrentIndex)
		CurrentIndex++;

	if (CurrentIndex > 4)
	{
		CurrentWeaponIndex = 0;
	}
	else
	{
		CurrentWeaponIndex = CurrentIndex;
	}

	UARItemWeapon* NextWeaponAbility = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!NextWeaponAbility)
	{
		NextWeaponAbility = FindNextValid();
	}
	
	if (WeaponIndex == CurrentWeaponIndex)
	{
		ClientNextWeapon(CurrentWeaponIndex, true);
	}
	else
	{
		ClientNextWeapon(CurrentWeaponIndex, false);
	}
}
bool UARWeaponInventoryComponent::ServerNextWeapon_Validate(uint8 WeaponIndex)
{
	return true;
}
void UARWeaponInventoryComponent::ClientNextWeapon_Implementation(uint8 WeaponIndex, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;

	CurrentWeaponIndex = WeaponIndex;
	UARItemWeapon* NextWeaponAbility = GetItem<UARItemWeapon>(WeaponIndex);
	Equip(WeaponIndex, NextWeaponAbility);
}

void UARWeaponInventoryComponent::ServerPreviousWeapon_Implementation(uint8 WeaponIndex)
{
	uint8 CurrentIndex = CurrentWeaponIndex;
	if (CurrentIndex > WeaponIndex)
		CurrentIndex--;

	if (CurrentIndex < 0)
	{
		CurrentWeaponIndex = 3;
	}
	else
	{
		CurrentWeaponIndex = CurrentIndex;
	}

	UARItemWeapon* NextWeaponAbility = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!NextWeaponAbility)
	{
		NextWeaponAbility = FindPreviousValid();
	}
	//so Server index is different. Client might tried to cheat
	//or sometrhing. We will override it.
	//situation where client can chage multiple weapons within second
	//should not have place, as there is animation and/or internal cooldown on weapon change.
	//since it will be done trough ability.
	if (CurrentWeaponIndex != WeaponIndex)
	{
		ClientPreviousWeapon(CurrentWeaponIndex, false);
	}
	else
	{
		ClientPreviousWeapon(CurrentWeaponIndex, true);
	}
}
bool UARWeaponInventoryComponent::ServerPreviousWeapon_Validate(uint8 WeaponIndex)
{
	return true;
}
void UARWeaponInventoryComponent::ClientPreviousWeapon_Implementation(uint8 WeaponIndex, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
		return;
}



UARItemWeapon* UARWeaponInventoryComponent::FindNextValid()
{
	UARItemWeapon* WeaponAbilityTag = nullptr;

	uint8 Idx = CurrentWeaponIndex;
	while (!WeaponAbilityTag)
	{
		Idx++;
		if (Idx > 4 - 1)
		{
			Idx = 0;
		}
		WeaponAbilityTag = GetItem<UARItemWeapon>(Idx);

		//no weapon at first index, just assume there is nothing equipped.
		if ((Idx == 0) && !WeaponAbilityTag)
		{
			break;
		}
	}
	if (WeaponAbilityTag)
	{
		CurrentWeaponIndex = Idx;
	}
	return WeaponAbilityTag;
}

UARItemWeapon* UARWeaponInventoryComponent::FindPreviousValid()
{
	UARItemWeapon* WeaponAbilityTag = nullptr;
	uint8 Idx = CurrentWeaponIndex;
	while (!WeaponAbilityTag)
	{
		Idx--;
		if (Idx < 0)
		{
			Idx = 4 - 1;
		}

		//again no ability for weapon.
		if ((Idx == (4 - 1)) && !WeaponAbilityTag)
		{
			break;
		}
		WeaponAbilityTag = GetItem<UARItemWeapon>(Idx);
	}
	if (WeaponAbilityTag)
	{
		CurrentWeaponIndex = Idx;
	}
	return WeaponAbilityTag;
}


void UARWeaponInventoryComponent::OnRep_Group001HolsteredAttachment()
{
	///if (Group001HolsteredAttachment.WeaponMesh.IsPending())
	///{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			SetWeapon(Group001HolsteredAttachment, Character->GetHolsteredRightWeapon());
		}
	///}
}
void UARWeaponInventoryComponent::OnRep_Group002HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group002HolsteredAttachment, Character->GetHolsteredLeftWeapon());
	}
}
void UARWeaponInventoryComponent::OnRep_Group003HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group003HolsteredAttachment, Character->GetHolsteredRightWeapon());
	}
}
void UARWeaponInventoryComponent::OnRep_Group004HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group004HolsteredAttachment, Character->GetHolsteredRightWeapon());
	}
}

void UARWeaponInventoryComponent::OnRep_MainHandWeapon(FARWeapon OldWeapon)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		//SetWeapon(OldWeapon, GroupToComponent[OldWeapon.Group]);
		SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
		//GroupToComponent[MainHandWeapon.Group]->SetChildActorClass(nullptr);
	}
}

void UARWeaponInventoryComponent::AsynWeaponLoaded(UChildActorComponent* Component, FARWeapon InWeapon)
{
	Component->SetChildActorClass(InWeapon.Weapon.Get());
	
	Component->SetRelativeLocation(FVector(0,0,0));
	Component->SetRelativeRotation(FRotator(0,0,0));

	Component->SetRelativeLocation(InWeapon.Position);
	Component->SetRelativeRotation(InWeapon.Rotation);
}