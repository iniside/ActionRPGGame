// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSCharacter.h"
#include "../Items/GSItemWeaponInfo.h"
#include "GSWeaponRanged.h"

#include "GSWeaponEquipmentComponent.h"

UGSWeaponEquipmentComponent::UGSWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	SocketToAttach = NAME_None;
}
void UGSWeaponEquipmentComponent::InitializeComponent()
{
	if (Inventory)
		Inventory->OnItemSlotSwapped.AddDynamic(this, &UGSWeaponEquipmentComponent::OnItemFromInventory);

	OnItemSlotSwapped.AddDynamic(this, &UGSWeaponEquipmentComponent::OnItemWeapEquip);
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	Super::InitializeComponent();
}
void UGSWeaponEquipmentComponent::OnItemFromInventory(const FGISSlotSwapInfo& SlotSwapInfoIn)
{

}
void UGSWeaponEquipmentComponent::OnItemWeapEquip(const FGISSlotSwapInfo& SlotSwapInfoIn)
{

}

bool UGSWeaponEquipmentComponent::CheckIfCanAddItemToSlot(class UGISItemData* TargetDataIn, class UGISItemData* LastDataIn)
{
	//assuming setup
	//two tabs left and right weapons
	//iterate over two tabs to check if incoming weapon meets criteria to be added.


	//another possiblty is to add tabs with two slots
	//0 - left, 1- right
	//each tab behave like weapon set
	//if weapon can be dual wield there can be two weapons in set
	//if weapon can takes both hands, there can be only one.

	//but none of this solve basic issue, which is attaching weapons
	//to proper sockets on character.
	//for example weapon can be dual wielded, but can also take back socket (like staff)
	//so essentialy we should limit equiped weapons by two factors:
	//1. Available attachment sockets
	//2. wheter or not dual wielding weapon is actually possible.
	//
	//For example staff which can be dual wielded, can also be only attached to back of character
	//which essentialy remove one slot for two handed weapon
	//as staff is as big as two handed weapon, but can be wieled in either two hands or one hand.
	//
	//So when player equip weapon, we should check all available slots, against weapon attachment slot
	//and then check if this weapon, can be equiped with another weapon.

	//how do we determine do which slot weapon should be attached ?
	//weapon have it's own list of possible slots
	//but idealy if weapon is equiped to left tab
	//we should attach to left sockets.
	//problems..
	UGSItemWeaponInfo* tarWeap = Cast<UGSItemWeaponInfo>(TargetDataIn);
	
	if (!tarWeap)
		return false;
	if (!tarWeap->Weapon)
		return false;
	
	//TArray<FGSWeaponSocketInfo> LeftFreeAttachments;
	//for (FGSWeaponSocketInfo& socket : LeftAttachmentSockets)
	//{
	//	if (socket.bIsSocketAvailable)
	//		LeftFreeAttachments.Add(socket);
	//}
	//TArray<FGSWeaponSocketInfo> RightFreeAttachments;
	//for (FGSWeaponSocketInfo& socket : RightAttachmentSockets)
	//{
	//	if (socket.bIsSocketAvailable)
	//		RightFreeAttachments.Add(socket);
	//}

	
	//look for any potentional free socket.
	//which essentialy equal free weapon slot.
	bool bFoundFreeSocket = false;
	FName FoundSocketName;
	const TArray<FGSWeaponSocketInfo>& weapSockets = tarWeap->GetPossibleWeaponSockets();
	int32 weapSockNum = weapSockets.Num();
	
	for (auto It = AttachmentSockets.CreateIterator(); It; ++It)
	{
		//for (FGSWeaponSocketInfo& socket : AttachmentSockets)
		//{
		if (It->bIsSocketAvailable)
		{
			//for (const FGSWeaponSocketInfo& socketWeap : weapSockets)
			//{
			for (int32 idx = 0; idx < weapSockNum; idx++)
			{
				if (It->SocketName == weapSockets[idx].SocketName)
				{
					bFoundFreeSocket = true;
					SocketToAttach = It->SocketName;
					It->bIsSocketAvailable = false;
					break;
				}
			}
			//}
		}
		//}
	}
	return bFoundFreeSocket;
}

void UGSWeaponEquipmentComponent::AttachActorTo(AActor* ActorIn, FName& AttachedSocketNameOut, const TArray<FGSWeaponSocketInfo>& PossibleSockets)
{
	if (EquipInt)
	{
		//TArray<FGSWeaponSocketInfo> AvailableSockets;

		//for (FGSWeaponSocketInfo& pawnSocket : AttachmentSockets)
		//{
		//	if (pawnSocket.bIsSocketAvailable)
		//	{
		//		AvailableSockets.Add(pawnSocket);
		//	}
		//}
		////iterate over all available sockets.
		//for (FGSWeaponSocketInfo& pawnSocket : AvailableSockets)
		//{
		//	//if not iterate over sockets, to which this weapon can be possibly attached.
		//	for (const FGSWeaponSocketInfo& weapSocket : PossibleSockets)
		//	{
		//		//if any of this socket matches available sockets attach weapon.
		//		if (pawnSocket.SocketName == weapSocket.SocketName)
		//		{
		AttachedSocketNameOut = SocketToAttach; //weapSocket.SocketName;
					//pawnSocket.bIsSocketAvailable = false;
		EquipInt->AttachActor(ActorIn, SocketToAttach);
		SocketToAttach = NAME_None;
					//return;
	//			}
	//		}
	//	}
	}
}
