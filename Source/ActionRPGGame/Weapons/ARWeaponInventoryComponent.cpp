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


	MaxSlots = 4;
	AvailableSlots = 4;

	// ...
}


// Called when the game starts
void UARWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UARWeaponInventoryComponent::InitializeWeapons(APawn* Pawn)
{
	if (AARCharacter* Character = Cast<AARCharacter>(Pawn))
	{
		GroupToComponent.Add(0, Character->GetHolsteredRightWeapon());
		GroupToComponent.Add(1, Character->GetHolsteredLeftWeapon());
		GroupToComponent.Add(2, Character->GetHolsteredBackDownWeapon());
		GroupToComponent.Add(3, Character->GetHolsteredSideLeftWeapon());
		POwner = Character;
	}
}
// Called every frame
void UARWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UARWeaponInventoryComponent::SetWeapon(const FARWeaponRPC& InWeapon, UChildActorComponent* Component)
{
	if (InWeapon.Weapon.IsValid() || InWeapon.Weapon.IsNull())
	{
		Component->SetChildActorClass(InWeapon.Weapon.Get());
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
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = InWeapon->HolsteredPosition;
	Data.Rotation = InWeapon->HolsteredRotation;
	Data.AttachSlot = static_cast<EARWeaponPosition>(LocalIndex);
	SetWeapon(Data, GroupToComponent[LocalIndex]);
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			PC->WeaponManager->NativeEquipAbility(InWeapon->Ability, static_cast<EAMGroup>(LocalIndex), EAMSlot::Slot001, false);
		}
	}
}
void UARWeaponInventoryComponent::OnItemRemoved(uint8 LocalIndex)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			PC->WeaponManager->NativeRemoveAbility(TSoftClassPtr<UGAAbilityBase>(), static_cast<EAMGroup>(LocalIndex), EAMSlot::Slot001);
		}
	}
	FARWeaponRPC Data;
	Data.Weapon.Reset();
	//Data.SocketName = InWeapon->Socket;
	Data.Position = FVector::ZeroVector;
	Data.Rotation = FRotator::ZeroRotator;
	Data.AttachSlot = static_cast<EARWeaponPosition>(LocalIndex);

	SetWeapon(Data, GroupToComponent[LocalIndex]);
	if (LocalIndex == CurrentWeaponIndex)
	{
		Unequip(LocalIndex);
	}
}

void UARWeaponInventoryComponent::OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex)
{
	UARItemWeapon* InWeapon = Cast<UARItemWeapon>(Item);
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = InWeapon->HolsteredPosition;
	Data.Rotation = InWeapon->HolsteredRotation;
	Data.AttachSlot = static_cast<EARWeaponPosition>(LocalIndex);

	MulticastAddWeapon(Data);
}
void UARWeaponInventoryComponent::OnServerItemRemoved(uint8 LocalIndex)
{
}
void UARWeaponInventoryComponent::MulticastAddWeapon_Implementation(const FARWeaponRPC& WeaponData)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		switch (WeaponData.AttachSlot)
		{
		case EARWeaponPosition::Right:
			SetWeapon(WeaponData, Character->GetHolsteredRightWeapon());
			break;
		case EARWeaponPosition::Left:
			SetWeapon(WeaponData, Character->GetHolsteredLeftWeapon());
			break;
		case EARWeaponPosition::BottomBack:
			SetWeapon(WeaponData, Character->GetHolsteredBackDownWeapon());
			break;
		case EARWeaponPosition::Side:
			SetWeapon(WeaponData, Character->GetHolsteredSideLeftWeapon());
			break;
		case EARWeaponPosition::Equiped:
			SetWeapon(WeaponData, Character->GetEquipedMainWeapon());
			break;
		default:
			break;
		}
		
	}
}
void UARWeaponInventoryComponent::MulticastRemoveWeapon_Implementation(const FARWeaponRPC& WeaponData)
{

}

void UARWeaponInventoryComponent::MulticastEquipWeapon_Implementation(uint8 WeaponIndex, const FARWeaponRPC& WeaponData)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(WeaponData, Character->GetEquipedMainWeapon());
		GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
	}
	CurrentWeaponIndex = WeaponIndex;
}

void UARWeaponInventoryComponent::Equip(uint8 WeaponIndex, const FARWeaponRPC& WeaponData)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(WeaponData, Character->GetEquipedMainWeapon());
		GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			UARItemWeapon* ItemWeapon = GetItem<UARItemWeapon>(WeaponIndex);
			FSoftObjectPath Path = ItemWeapon->Ability.ToSoftObjectPath();
			TSoftClassPtr<UGAAbilityBase> ab(Path);
			PC->WeaponManager->EquipWeapon(ab);
		}
		CurrentWeaponIndex = WeaponIndex;
	}
}
void UARWeaponInventoryComponent::Unequip(uint8 WeaponIndex)
{
	//MainHandWeapon.Weapon.Reset();
	//MainHandWeapon.Position = FVector(0,0,0);
	//MainHandWeapon.Rotation = FRotator(0,0,0);
	//MainHandWeapon.NetIndex = 0;
	//MainHandWeapon.RepCounter++;
	//WeaponHelper[WeaponIndex]->Weapon.Reset();
	//WeaponHelper[WeaponIndex]->RepCounter++;

	//if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	//{
	//	SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
	//	GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
	//	if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
	//	{
	//		PC->WeaponManager->RemoveAbility(static_cast<EAMGroup>(WeaponIndex), EAMSlot::Slot001);
	//	}
	//}
}
void UARWeaponInventoryComponent::Holster()
{
	UARItemWeapon* EquipedWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!EquipedWeapon)
		return;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		Character->GetAbilityComp()->RemoveAbilitiesFromActions(EquipedWeapon->Ability);
		Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
	}
	FARWeaponRPC Data;
	Data.Weapon = EquipedWeapon->Weapon;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = EquipedWeapon->HolsteredPosition;
	Data.Rotation = EquipedWeapon->HolsteredRotation;
	Data.AttachSlot = static_cast<EARWeaponPosition>(CurrentWeaponIndex);
	SetWeapon(Data, GroupToComponent[CurrentWeaponIndex]);
	ServerHolster(Data);
}
void UARWeaponInventoryComponent::ServerHolster_Implementation(const FARWeaponRPC& WeaponData)
{
	MulticastHolster(WeaponData);
}
bool UARWeaponInventoryComponent::ServerHolster_Validate(const FARWeaponRPC& WeaponData)
{
	return true;
}
void UARWeaponInventoryComponent::MulticastHolster_Implementation(const FARWeaponRPC& WeaponData)
{
	SetWeapon(WeaponData, GroupToComponent[CurrentWeaponIndex]);
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

	UARItemWeapon* InWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!InWeapon)
	{
		InWeapon = FindNextValid();
	}
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = InWeapon->EquipedPosition;
	Data.Rotation = InWeapon->EquipedRotation;
	Data.AttachSlot = EARWeaponPosition::Equiped;
	Equip(CurrentWeaponIndex, Data);
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
	Holster();
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

	UARItemWeapon* InWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!InWeapon)
	{
		InWeapon = FindNextValid();
	}
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = InWeapon->EquipedPosition;
	Data.Rotation = InWeapon->EquipedRotation;
	Data.AttachSlot = EARWeaponPosition::Equiped;

	MulticastEquipWeapon(CurrentWeaponIndex, Data);
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
	//Equip(WeaponIndex, NextWeaponAbility);
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
	//Equip(CurrentWeaponIndex, NextWeaponAbility);
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

void UARWeaponInventoryComponent::AsynWeaponLoaded(UChildActorComponent* Component, FARWeaponRPC InWeapon)
{
	Component->SetChildActorClass(InWeapon.Weapon.Get());
	
	Component->SetRelativeLocation(FVector(0,0,0));
	Component->SetRelativeRotation(FRotator(0,0,0));

	Component->SetRelativeLocation(InWeapon.Position);
	Component->SetRelativeRotation(InWeapon.Rotation);
}
void UARWeaponInventoryComponent::AddMagazineMod(uint8 WeaponIdx, uint8 MagazineModIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddMagazineMod(WeaponIdx, MagazineModIndex);
		return;
	}

	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			UIFInventoryComponent* MainInventory = PC->MainInventory;

			UARMagazineUpgradeItem* Magazine = MainInventory->GetItem<UARMagazineUpgradeItem>(MagazineModIndex);
			if (Magazine)
			{
				UARItemWeapon* Weapon = GetItem<UARItemWeapon>(WeaponIdx);
				if (Weapon)
				{
					Weapon->AddMagazineUpgrade(Magazine);
					MainInventory->RemoveItem(MagazineModIndex);
				}
			}
		}
	}
}
void UARWeaponInventoryComponent::ServerAddMagazineMod_Implementation(uint8 WeaponIdx, uint8 MagazineModIndex)
{
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			UIFInventoryComponent* MainInventory = PC->MainInventory;

			UARMagazineUpgradeItem* Magazine = MainInventory->GetItem<UARMagazineUpgradeItem>(MagazineModIndex);
			if (Magazine)
			{
				UARItemWeapon* Weapon = GetItem<UARItemWeapon>(WeaponIdx);
				if (Weapon)
				{
					Weapon->AddMagazineUpgrade(Magazine);
					MainInventory->RemoveItem(MagazineModIndex);
				}
			}
		}
	}
}
bool UARWeaponInventoryComponent::ServerAddMagazineMod_Validate(uint8 WeaponIdx, uint8 MagazineModIndex)
{
	return true;
}