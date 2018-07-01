// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponInventoryComponent.h"
#include "Engine/AssetManager.h"
#include "ARItemWeapon.h"
#include "Weapons/ARWeaponAbilityBase.h"
#include "ARCharacter.h"
#include "ARPlayerController.h"
#include "AFAbilityComponent.h"

// Sets default values for this component's properties
UARWeaponInventoryComponent::UARWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	MaxSlots = 4;
	AvailableSlots = 4;
	ClientWeaponAbilities.SetNum(4);
	ServerWeaponAbilities.SetNum(4);
	CurrentWeaponIndex = -1;
	// ...
}


// Called when the game starts
void UARWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void UARWeaponInventoryComponent::BindInputs(UInputComponent* InputComponent, class UAFAbilityComponent* AbilityComponent)
{
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
	//if (InWeapon.Weapon.IsValid() || InWeapon.Weapon.IsNull())
	{
		TSubclassOf<AARWeaponBase> WpnClass = TSoftClassPtr<AARWeaponBase>(InWeapon.Weapon).LoadSynchronous();
		Component->SetChildActorClass(WpnClass);
		Component->SetRelativeLocation(FVector(0, 0, 0));
		Component->SetRelativeRotation(FRotator(0, 0, 0));

		Component->SetRelativeLocation(InWeapon.Position);
		Component->SetRelativeRotation(InWeapon.Rotation);
	}
}
void UARWeaponInventoryComponent::OnClientPreItemAdded(UIFItemBase* Item, uint8 Index)
{
	if (UARItemWeapon* ItemWeapon = Cast<UARItemWeapon>(Item))
	{

	}
}
void UARWeaponInventoryComponent::OnItemAdded(UIFItemBase* Item, uint8 LocalIndex)
{
	if (UARItemWeapon* ItemWeapon = Cast<UARItemWeapon>(Item))
	{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			UAFAbilityComponent* AbilityComp = Character->GetAbilityComp();
			if (!AbilityComp)
				return;
			FAFOnAbilityReady Del = FAFOnAbilityReady::CreateUObject(this, &UARWeaponInventoryComponent::OnAbilityAdded);
			AbilityComp->AddOnAbilityReadyDelegate(ItemWeapon->AbilityHandle, Del);

			ClientWeaponAbilities[LocalIndex] = ItemWeapon->AbilityHandle;
			//handle case of Client/Server ability handle.
			AbilityComp->NativeAddAbility(ItemWeapon->Ability, ItemWeapon->AbilityHandle);
		}
	}
}
void UARWeaponInventoryComponent::OnAbilityAdded(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	int32 Idx = ClientWeaponAbilities.IndexOfByKey(ClientHandle);
	ServerWeaponAbilities[Idx] = ServerHandle;
}
void UARWeaponInventoryComponent::OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex)
{
	if (UARItemWeapon* ItemWeapon = Cast<UARItemWeapon>(Item))
	{
		ServerWeaponAbilities[LocalIndex] = ItemWeapon->AbilityHandle;
	}
}
void UARWeaponInventoryComponent::OnWeaponReady(TSoftClassPtr<UARWeaponAbilityBase> InAbilityTag, int8 LocalIndex)
{
	AARCharacter* Character = Cast<AARCharacter>(POwner);
	if (!Character)
		return;

	UAFAbilityComponent* AbilityComp = Character->GetAbilityComp();

	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByHandle(ServerWeaponAbilities[LocalIndex]));
	SetAbilityToItem(LocalIndex, Ability);
}
void UARWeaponInventoryComponent::SetAbilityToItem(int8 InLocalIndex, class UGAAbilityBase* InAbility)
{
	UARItemWeapon* ItemWeapon = GetItem<UARItemWeapon>(InLocalIndex);

	if (!ItemWeapon)
		return;

	ItemWeapon->SetAbility(Cast<UARWeaponAbilityBase>(InAbility));
}
void UARWeaponInventoryComponent::OnItemRemoved(uint8 LocalIndex)
{
	AARCharacter* Character = Cast<AARCharacter>(POwner);

	//Character->GetAbilityComp()->NativeRemoveAbility(WeaponAbilities[LocalIndex]);

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


void UARWeaponInventoryComponent::OnServerItemRemoved(uint8 LocalIndex)
{
	FARWeaponRPC Data;
	Data.Weapon.Reset();
	Data.Index = LocalIndex;
	//Data.SocketName = InWeapon->Socket;
	Data.Position = FVector::ZeroVector;
	Data.Rotation = FRotator::ZeroRotator;
	Data.AttachSlot = static_cast<EARWeaponPosition>(LocalIndex);
	MulticastRemoveWeapon(Data);
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
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		switch (WeaponData.AttachSlot)
		{
		case EARWeaponPosition::Right:
			Character->GetHolsteredRightWeapon()->SetChildActorClass(nullptr);
			break;
		case EARWeaponPosition::Left:
			Character->GetHolsteredLeftWeapon()->SetChildActorClass(nullptr);
			break;
		case EARWeaponPosition::BottomBack:
			Character->GetHolsteredBackDownWeapon()->SetChildActorClass(nullptr);
			break;
		case EARWeaponPosition::Side:
			Character->GetHolsteredSideLeftWeapon()->SetChildActorClass(nullptr);
			break;
		case EARWeaponPosition::Equiped:
			Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
			break;
		default:
			break;
		}
	}

	if (WeaponData.Index == CurrentWeaponIndex)
	{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
		}
	}
}

void UARWeaponInventoryComponent::MulticastEquipWeapon_Implementation(int8 WeaponIndex, const FARWeaponRPC& WeaponData)
{
	ENetRole Role = GetOwnerRole();
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(WeaponData, Character->GetEquipedMainWeapon());
		GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
	}
	CurrentWeaponIndex = WeaponIndex;
}
void UARWeaponInventoryComponent::MulticastUnequipWeapon_Implementation(int8 OldWeaponIndex, const FARWeaponRPC& WeaponData)
{
	UARItemWeapon* EquipedWeapon = GetItem<UARItemWeapon>(OldWeaponIndex);
	if (!EquipedWeapon)
		return;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		if (!Character->GetEquipedMainWeapon()->GetChildActor())
		{
			return;
		}
		Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
	}
	SetWeapon(WeaponData, GroupToComponent[OldWeaponIndex]);
}
void UARWeaponInventoryComponent::Equip(int8 WeaponIndex, const FARWeaponRPC& WeaponData)
{
	if (WeaponIndex < 0)
		return;

	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(WeaponData, Character->GetEquipedMainWeapon());
		GroupToComponent[WeaponIndex]->SetChildActorClass(nullptr);
		CurrentWeaponIndex = WeaponIndex;
	}
}
void UARWeaponInventoryComponent::Unequip(int8 WeaponIndex)
{
	UARItemWeapon* EquipedWeapon = GetItem<UARItemWeapon>(WeaponIndex);
	if (!EquipedWeapon)
		return;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		if (!Character->GetEquipedMainWeapon()->GetChildActor())
		{
			return;
		}
		Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
	}
	FARWeaponRPC Data;
	Data.Weapon = EquipedWeapon->Weapon.ToString();
	//Data.SocketName = InWeapon->Socket;
	Data.Position = EquipedWeapon->HolsteredPosition;
	Data.Rotation = EquipedWeapon->HolsteredRotation;
	Data.AttachSlot = static_cast<EARWeaponPosition>(WeaponIndex);
	SetWeapon(Data, GroupToComponent[WeaponIndex]);
	ServerHolster(Data);
	CurrentWeaponIndex = -1;
}
void UARWeaponInventoryComponent::Holster()
{
	UARItemWeapon* EquipedWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!EquipedWeapon)
		return;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
	}
	FARWeaponRPC Data;
	Data.Weapon = EquipedWeapon->Weapon.ToString();
	//Data.SocketName = InWeapon->Socket;
	Data.Position = EquipedWeapon->HolsteredPosition;
	Data.Rotation = EquipedWeapon->HolsteredRotation;
	Data.AttachSlot = static_cast<EARWeaponPosition>(CurrentWeaponIndex);
	SetWeapon(Data, GroupToComponent[CurrentWeaponIndex]);
	ServerHolster(Data);
	CurrentWeaponIndex = -1;
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
	ENetRole Role = GetOwnerRole();
	if (Role != ENetRole::ROLE_AutonomousProxy
		&&  Role != ENetRole::ROLE_Authority)
	{
		SetWeapon(WeaponData, GroupToComponent[CurrentWeaponIndex]);
		CurrentWeaponIndex = -1;
	}
}

void UARWeaponInventoryComponent::NextWeapon()
{
	ENetMode NetMode = GetOwner()->GetNetMode();
	bool bProceed = false;
	switch (NetMode)
	{
	case NM_Standalone:
		bProceed = true;
		break;
	case NM_DedicatedServer:
		break;
	case NM_ListenServer:
		break;
	case NM_Client:
		bProceed = true;
		break;
	case NM_MAX:
		break;
	default:
		break;
	}

	if (!bProceed)
	{
		return;
	}

	int8 OldGroup = CurrentWeaponIndex;
	if (OldGroup  > -1)
		Unequip(OldGroup);
	
	int8 CurrentIndex = CurrentWeaponIndex;
	CurrentIndex++;
	if (CurrentIndex > 4 || CurrentIndex < 0)
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
	if (!InWeapon)
	{
		//we don't have any weapon.
		return;
	}
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon.ToString();
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
	ENetMode NetMode = GetOwner()->GetNetMode();
	bool bProceed = false;
	switch (NetMode)
	{
	case NM_Standalone:
		bProceed = true;
		break;
	case NM_DedicatedServer:
		break;
	case NM_ListenServer:
		break;
	case NM_Client:
		bProceed = true;
		break;
	case NM_MAX:
		break;
	default:
		break;
	}

	if (!bProceed)
	{
		return;
	}
	int8 OldGroup = CurrentWeaponIndex;

	int8 CurrentIndex = CurrentWeaponIndex;
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
void UARWeaponInventoryComponent::ServerNextWeapon_Implementation(int8 WeaponIndex)
{
	int8 OldGroup = CurrentWeaponIndex;

	int8 CurrentIndex = CurrentWeaponIndex;
	//if (WeaponIndex > CurrentIndex)
		CurrentIndex++;

	if (CurrentIndex > 4 || CurrentIndex < 0)
	{
		CurrentWeaponIndex = 0;
	}
	else
	{
		CurrentWeaponIndex = CurrentIndex;
	}

	if (OldGroup > -1)
	{
		UARItemWeapon* OldWeapon = GetItem<UARItemWeapon>(OldGroup);
		if (OldWeapon)
		{
			FARWeaponRPC OldData;
			OldData.Weapon = OldWeapon->Weapon.ToString();
			OldData.Position = OldWeapon->HolsteredPosition;
			OldData.Rotation = OldWeapon->HolsteredRotation;
			OldData.AttachSlot = static_cast<EARWeaponPosition>(OldGroup);
			MulticastUnequipWeapon(OldGroup, OldData);
		}
	}

	UARItemWeapon* InWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!InWeapon)
	{
		InWeapon = FindNextValid();
	}
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon.ToString();
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
bool UARWeaponInventoryComponent::ServerNextWeapon_Validate(int8 WeaponIndex)
{
	return true;
}
void UARWeaponInventoryComponent::ClientNextWeapon_Implementation(int8 WeaponIndex, bool bPredictionSuccess)
{
	HandleClientPrediction(WeaponIndex, bPredictionSuccess);
}

void UARWeaponInventoryComponent::ServerPreviousWeapon_Implementation(int8 WeaponIndex)
{
	int8 CurrentIndex = CurrentWeaponIndex;
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

	UARItemWeapon* OldWeapon = GetItem<UARItemWeapon>(CurrentIndex);
	if (OldWeapon)
	{
		FARWeaponRPC OldData;
		OldData.Weapon = OldWeapon->Weapon.ToString();
		OldData.Position = OldWeapon->HolsteredPosition;
		OldData.Rotation = OldWeapon->HolsteredRotation;
		OldData.AttachSlot = static_cast<EARWeaponPosition>(CurrentIndex);
		MulticastUnequipWeapon(CurrentIndex, OldData);
	}

	UARItemWeapon* InWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
	if (!InWeapon)
	{
		InWeapon = FindNextValid();
	}
	FARWeaponRPC Data;
	Data.Weapon = InWeapon->Weapon.ToString();
	Data.Position = InWeapon->EquipedPosition;
	Data.Rotation = InWeapon->EquipedRotation;
	Data.AttachSlot = EARWeaponPosition::Equiped;
	MulticastEquipWeapon(CurrentWeaponIndex, Data);

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
bool UARWeaponInventoryComponent::ServerPreviousWeapon_Validate(int8 WeaponIndex)
{
	return true;
}
void UARWeaponInventoryComponent::ClientPreviousWeapon_Implementation(int8 WeaponIndex, bool bPredictionSuccess)
{
	HandleClientPrediction(WeaponIndex, bPredictionSuccess);
}

void UARWeaponInventoryComponent::HandleClientPrediction(int8 WeaponIndex, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
	{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			UARItemWeapon* Item = GetItem<UARItemWeapon>(WeaponIndex);

			UAFAbilityComponent* AbilityComp = Character->GetAbilityComp();
			if (!AbilityComp)
				return;
			//TODO
			TArray<uint8> InputIDs;
			InputIDs.Add(static_cast<uint8>(AbilityInput::Shoot));
			InputIDs.Add(static_cast<uint8>(AbilityInput::Reload));
			AbilityComp->BindAbilityToInputIDs(ServerWeaponAbilities[WeaponIndex], InputIDs);
		}
		return;
	}
	else //override client decision.
	{
		CurrentWeaponIndex = WeaponIndex;
		UARItemWeapon* InWeapon = GetItem<UARItemWeapon>(CurrentWeaponIndex);
		if (!InWeapon)
		{
			InWeapon = FindNextValid();
		}
		FARWeaponRPC Data;
		Data.Weapon = InWeapon->Weapon.ToString();
		//Data.SocketName = InWeapon->Socket;
		Data.Position = InWeapon->EquipedPosition;
		Data.Rotation = InWeapon->EquipedRotation;
		Data.AttachSlot = EARWeaponPosition::Equiped;

		Equip(CurrentWeaponIndex, Data);

		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			UAFAbilityComponent* AbilityComp = Character->GetAbilityComp();
			if (!AbilityComp)
				return;
			//TODO
			TArray<uint8> InputIDs;
			InputIDs.Add(static_cast<uint8>(AbilityInput::Shoot));
			InputIDs.Add(static_cast<uint8>(AbilityInput::Reload));
			AbilityComp->BindAbilityToInputIDs(ServerWeaponAbilities[WeaponIndex], InputIDs);
		}
	}
}

UARItemWeapon* UARWeaponInventoryComponent::FindNextValid()
{
	UARItemWeapon* WeaponAbilityTag = nullptr;

	int8 Idx = CurrentWeaponIndex;
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
	int8 Idx = CurrentWeaponIndex;
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
	Component->SetChildActorClass(TSoftClassPtr<AARWeaponBase>(InWeapon.Weapon).LoadSynchronous());
	
	Component->SetRelativeLocation(FVector(0,0,0));
	Component->SetRelativeRotation(FRotator(0,0,0));

	Component->SetRelativeLocation(InWeapon.Position);
	Component->SetRelativeRotation(InWeapon.Rotation);
}
void UARWeaponInventoryComponent::AddMagazineMod(int8 WeaponIdx, int8 MagazineModIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddMagazineMod(WeaponIdx, MagazineModIndex);
		return;
	}
}
void UARWeaponInventoryComponent::ServerAddMagazineMod_Implementation(int8 WeaponIdx, int8 MagazineModIndex)
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
					ClientAddMagazineMod(WeaponIdx, MagazineModIndex);
				}
			}
		}
	}
}
bool UARWeaponInventoryComponent::ServerAddMagazineMod_Validate(int8 WeaponIdx, int8 MagazineModIndex)
{
	return true;
}

void UARWeaponInventoryComponent::ClientAddMagazineMod_Implementation(int8 WeaponIdx, int8 MagazineModIndex)
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

					OnUpgradeInstalled.Broadcast(Weapon, Magazine, WeaponIdx);
				}
			}
		}
	}
}

void UARWeaponInventoryComponent::RemoveMagazineMod(int8 WeaponIdx)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerRemoveMagazineMod(WeaponIdx);
		return;
	}

}
void UARWeaponInventoryComponent::ServerRemoveMagazineMod_Implementation(int8 WeaponIdx)
{
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			UIFInventoryComponent* MainInventory = PC->MainInventory;

			UARItemWeapon* Weapon = GetItem<UARItemWeapon>(WeaponIdx);
			if (Weapon)
			{
				UARMagazineUpgradeItem* MagazineUpgrade = Weapon->RemoveMagazineUpgrade();
				ServerRemoveMagazineMod(WeaponIdx);
				PC->MainInventory->AddItemAnySlot(MagazineUpgrade);
				ClientRemoveMagazineMod(WeaponIdx);
			}
		}
	}
}
bool UARWeaponInventoryComponent::ServerRemoveMagazineMod_Validate(int8 WeaponIdx)
{
	return true;
}
void UARWeaponInventoryComponent::ClientRemoveMagazineMod_Implementation(int8 WeaponIdx)
{
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
		{
			UIFInventoryComponent* MainInventory = PC->MainInventory;

			UARItemWeapon* Weapon = GetItem<UARItemWeapon>(WeaponIdx);
			if (Weapon)
			{
				UARMagazineUpgradeItem* MagazineUpgrade = Weapon->RemoveMagazineUpgrade();
				OnUpgradeRemoved.Broadcast(Weapon, MagazineUpgrade, WeaponIdx);
			}
		}
	}
}