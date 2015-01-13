// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSItem.h"
#include "GSEquipmentComponent.h"
#include "../Weapons/GSWeaponEquipmentComponent.h"
#include "../Weapons/GSWeaponRanged.h"

#include "Net/UnrealNetwork.h"

#include "GSItemWeaponInfo.h"

UGSItemWeaponInfo::UGSItemWeaponInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	LastAttachedSocket = NAME_None;
}

void UGSItemWeaponInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGSItemWeaponInfo, Weapon, COND_OwnerOnly);
	DOREPLIFETIME(UGSItemWeaponInfo, ActiveWeapon);
}

UTexture2D* UGSItemWeaponInfo::GetImage()
{
	if (Weapon.GetDefaultObject())
	{
		return Weapon.GetDefaultObject()->Icon;
	}
	return nullptr;
}

bool UGSItemWeaponInfo::OnItemAddedToSlot()
{
	if (Weapon)
	{
		if (UGSWeaponEquipmentComponent* eqComp = Cast<UGSWeaponEquipmentComponent>(CurrentInventory))
		{
			ActiveWeapon = GetWorld()->SpawnActor<AGSWeaponRanged>(Weapon);
			ActiveWeapon->SetOwner(OwningPawn);
			ActiveWeapon->Instigator = OwningPawn;

			eqComp->AttachActorTo(ActiveWeapon, LastAttachedSocket, ActiveWeapon->SocketList);
		}
	}
	return false;
}
bool UGSItemWeaponInfo::OnItemRemovedFromSlot()
{
	if (ActiveWeapon)
	{
		UGSWeaponEquipmentComponent* eqComp = Cast<UGSWeaponEquipmentComponent>(CurrentInventory);
		if (eqComp)
		{
			for (FGSWeaponSocketInfo& socket : eqComp->AttachmentSockets)
			{
				if (socket.SocketName == LastAttachedSocket)
					socket.bIsSocketAvailable = true;
			}
		}
		if (!eqComp)
		{
			UGSWeaponEquipmentComponent* eqLastComp = Cast<UGSWeaponEquipmentComponent>(LastInventory);
			if (eqLastComp)
			{
				for (FGSWeaponSocketInfo& socket : eqLastComp->AttachmentSockets)
				{
					if (socket.SocketName == LastAttachedSocket)
						socket.bIsSocketAvailable = true;

					LastAttachedSocket = NAME_Name;
				}
			}
			ActiveWeapon->Destroy();
		}
		return true;
	}
	return false;
}

bool UGSItemWeaponInfo::InputPressed_Implementation()
{
	if (ActiveWeapon)
	{
		ActiveWeapon->InputPressed();
		return true;
	}
	return false;
}
bool UGSItemWeaponInfo::InputReleased_Implementation()
{
	if (ActiveWeapon)
	{
		ActiveWeapon->InputReleased();
		return true;
	}
	return false;
}

bool UGSItemWeaponInfo::CanItemBeSwapped()
{
	return true;
}