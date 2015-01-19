// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSItem.h"
#include "GSEquipmentComponent.h"
#include "../Weapons/GSWeaponEquipmentComponent.h"
#include "../Weapons/GSWeaponRanged.h"
#include "../Components/GSActiveActionsComponent.h"
#include "Net/UnrealNetwork.h"

#include "GSItemWeaponRangedInfo.h"

UGSItemWeaponRangedInfo::UGSItemWeaponRangedInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	LastAttachedSocket = NAME_None;
	CurrentHand = EGSWeaponHand::Invalid;
}
void UGSItemWeaponRangedInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGSItemWeaponRangedInfo, RangedWeaponClass, COND_OwnerOnly);
	DOREPLIFETIME(UGSItemWeaponRangedInfo, RangedWeapon);
}
void UGSItemWeaponRangedInfo::SetEquipingTime(float TimeIn)
{
	if (RangedWeapon)
		RangedWeapon->SetEquipingTime(TimeIn);
}
int32 UGSItemWeaponRangedInfo::GetCurrentMagainzeAmmo()
{
	if (RangedWeapon)
	{
		return RangedWeapon->GetRemainingMagazineAmmo();
	}
	return 0;
}
int32 UGSItemWeaponRangedInfo::GetCurrentTotalAmmo()
{
	if (RangedWeapon)
	{
		return RangedWeapon->GetRemaningAmmo();
	}
	return 0;
}
UTexture2D* UGSItemWeaponRangedInfo::GetImage()
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		return RangedWeaponClass.GetDefaultObject()->Icon;
	}
	return nullptr;
}

AActor* UGSItemWeaponRangedInfo::GetActorToAttach()
{
	return RangedWeapon;
}

const TArray<FGSWeaponSocketInfo>& UGSItemWeaponRangedInfo::GetPossibleWeaponSockets() const
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		return RangedWeaponClass.GetDefaultObject()->SocketList;
	}
	return socketArray;
}
const EGSWeaponWield UGSItemWeaponRangedInfo::GetWeaponWield() const
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		return RangedWeaponClass.GetDefaultObject()->WieldType;
	}
	return EGSWeaponWield::Invalid;
}
const EGSWeaponType UGSItemWeaponRangedInfo::GetWeaponType() const
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		return RangedWeaponClass.GetDefaultObject()->WeaponType;
	}
	return EGSWeaponType::Invalid;
}
FVector UGSItemWeaponRangedInfo::GetCrosshairStartLocation()
{
	if (RangedWeapon)
		return RangedWeapon->GetWeaponSocketLocation();
	return FVector::ZeroVector;
}

UAnimSequence* UGSItemWeaponRangedInfo::GetEquipedAnimation()
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		switch (CurrentHand)
		{
		case EGSWeaponHand::Left:
			return RangedWeaponClass.GetDefaultObject()->LeftHandAnimation;
		case EGSWeaponHand::Right:
			return RangedWeaponClass.GetDefaultObject()->RightHandAnimation;
		case EGSWeaponHand::BothHands:
			return RangedWeaponClass.GetDefaultObject()->BothHandsAnimation;
		case EGSWeaponHand::Invalid:
			return nullptr;
		default:
			return nullptr;
		}
	}
	return nullptr;
}
UAimOffsetBlendSpace* UGSItemWeaponRangedInfo::GetEquipedAimBlendSpace()
{
	if (RangedWeaponClass.GetDefaultObject())
	{
		return RangedWeaponClass.GetDefaultObject()->AimOffsetBlend;
		//switch (CurrentHand)
		//{
		//case EGSWeaponHand::Left:
		//	return RangedWeaponClass.GetDefaultObject()->LeftHandAnimation;
		//case EGSWeaponHand::Right:
		//	return RangedWeaponClass.GetDefaultObject()->RightHandAnimation;
		//case EGSWeaponHand::BothHands:
		//	return RangedWeaponClass.GetDefaultObject()->BothHandsAnimation;
		//case EGSWeaponHand::Invalid:
		//	return nullptr;
		//default:
		//	return nullptr;
		//}
	}
	return nullptr;
}
bool UGSItemWeaponRangedInfo::OnItemAddedToSlot()
{
	if (RangedWeaponClass)
	{
		if (UGSWeaponEquipmentComponent* eqComp = Cast<UGSWeaponEquipmentComponent>(CurrentInventory))
		{
			RangedWeapon = GetWorld()->SpawnActor<AGSWeaponRanged>(RangedWeaponClass);
			RangedWeapon->SetOwner(OwningPawn);
			RangedWeapon->Instigator = OwningPawn;
			RangedWeapon->WeaponPC = OwiningPlayerController;

			//we will use wepon defaults if it zero.
			if (RemainingAmmoMagazine != 0)
				RangedWeapon->SetRemainingMagazineAmmo(RemainingAmmoMagazine);
			if (RemainingAmmoTotal != 0)
				RangedWeapon->SetRemaningAmmofloat(RemainingAmmoTotal);

			RangedWeapon->InitializeWeapon();
			eqComp->AttachActorTo(RangedWeapon, LastAttachedSocket, RangedWeapon->SocketList);
		}
	}
	return false;
}
bool UGSItemWeaponRangedInfo::OnItemRemovedFromSlot()
{
	if (RangedWeapon)
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
			RemainingAmmoMagazine = RangedWeapon->GetRemainingMagazineAmmo();
			RemainingAmmoTotal = RangedWeapon->GetRemaningAmmo();
			RangedWeapon->Destroy();
		}
		return true;
	}
	return false;
}

bool UGSItemWeaponRangedInfo::InputPressed_Implementation()
{
	if (RangedWeapon)
	{
		RangedWeapon->InputPressed();
		return true;
	}
	return false;
}
bool UGSItemWeaponRangedInfo::InputReleased_Implementation()
{
	if (RangedWeapon)
	{
		RangedWeapon->InputReleased();
		return true;
	}
	return false;
}
void UGSItemWeaponRangedInfo::InputRealadWeapon()
{
	if (RangedWeapon)
	{
		RangedWeapon->InputReload();
	}
}