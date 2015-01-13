// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "../Items/GSItemWeaponInfo.h"
#include "../Items/IGSEquipment.h"
#include "../Weapons/GSWeaponRanged.h"
#include "../Weapons/GSWeaponEquipmentComponent.h"

#include "../GSCharacter.h" //temp!

#include "GSActiveActionsComponent.h"

UGSActiveActionsComponent::UGSActiveActionsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	LastRightCopiedIndex = -1;
	LastLeftCopiedIndex = -1;
}
void UGSActiveActionsComponent::InitializeComponent()
{
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	Super::InitializeComponent();
}
void UGSActiveActionsComponent::GetNextWeapon()
{

}
void UGSActiveActionsComponent::OnWeaponActive()
{

}
void UGSActiveActionsComponent::CopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerCopyNextItemFromOtherInventoryTab(OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex, WeaponHandIn);
	}
	else
	{
		if (OtherIn)
		{
			if (WeaponHandIn == EGSWeaponHand::Left)
			{
				int32 OtherTabSlotCount = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots.Num();
				for (int32 TabIndex = 0; TabIndex < OtherTabSlotCount; TabIndex++)
				{
					if (LastLeftCopiedIndex < TabIndex)
					{
						LastLeftCopiedIndex++;
						UGISItemData* LastData = Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData;
						Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots[TabIndex].ItemData;
						CurrentLeftHandWeapon = Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData;
						if (Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData)
						{
							Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->CurrentInventory = this;
							Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->LastInventory = OtherIn;
						}

						OnActionCopiedFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData, LastData, WeaponHandIn);

						TabUpdateInfo.ReplicationCounter++;
						TabUpdateInfo.TargetTabIndex = TargetTabIndex;
						if (GetNetMode() == ENetMode::NM_Standalone)
							OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);

						if (LastLeftCopiedIndex == (OtherTabSlotCount - 1))
							LastLeftCopiedIndex = -1;

						return;
					}
				}
			}
			else if (WeaponHandIn == EGSWeaponHand::Right)
			{
				int32 OtherTabSlotCount = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots.Num();
				for (int32 TabIndex = 0; TabIndex < OtherTabSlotCount; TabIndex++)
				{
					if (LastRightCopiedIndex < TabIndex)
					{
						LastRightCopiedIndex++;
						UGISItemData* LastData = Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData;
						Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots[TabIndex].ItemData;
						CurrentRightHandWeapon = Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData;
						if (Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData)
						{
							Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->CurrentInventory = this;
							Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->LastInventory = OtherIn;
						}

						OnActionCopiedFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData, LastData, WeaponHandIn);

						TabUpdateInfo.ReplicationCounter++;
						TabUpdateInfo.TargetTabIndex = TargetTabIndex;
						if (GetNetMode() == ENetMode::NM_Standalone)
							OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);

						if (LastRightCopiedIndex == (OtherTabSlotCount - 1))
							LastRightCopiedIndex = -1;

						return;
					}
				}
			}
		}
	}
}
void UGSActiveActionsComponent::ServerCopyNextItemFromOtherInventoryTab_Implementation(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn)
{
	CopyNextItemFromOtherInventoryTab(OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex, WeaponHandIn);
}
bool UGSActiveActionsComponent::ServerCopyNextItemFromOtherInventoryTab_Validate(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn)
{
	return true;
}

void UGSActiveActionsComponent::OnActionCopiedFromOtherInventoryTab(class UGISItemData* DataIn, class UGISItemData* LastDataIn, EGSWeaponHand WeaponHandIn)
{
	if (EquipInt)
	{

		UGSItemWeaponInfo* weapon = Cast<UGSItemWeaponInfo>(DataIn);
		UGSItemWeaponInfo* lastWeapon = Cast<UGSItemWeaponInfo>(LastDataIn);

		if (weapon)
		{
			if ( weapon->WeaponWieldType == EGSWeaponWield::TwoHands)
			{
				UGSItemWeaponInfo* leftWeapon = Cast<UGSItemWeaponInfo>(CurrentLeftHandWeapon);
				UGSItemWeaponInfo* RightWeapon = Cast<UGSItemWeaponInfo>(CurrentRightHandWeapon);
				if (WeaponHandIn == EGSWeaponHand::Right && leftWeapon)
				{
					EquipInt->AttachActor(leftWeapon->ActiveWeapon, leftWeapon->LastAttachedSocket);
					Tabs.InventoryTabs[1].TabSlots[0].ItemData = nullptr;
					CurrentRightHandWeapon = nullptr;
					TabUpdateInfo.ReplicationCounter++;
					TabUpdateInfo.TargetTabIndex = 1;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
					//put back weapon
				}
				if (WeaponHandIn == EGSWeaponHand::Left && RightWeapon)
				{
					EquipInt->AttachActor(RightWeapon->ActiveWeapon, RightWeapon->LastAttachedSocket);
					Tabs.InventoryTabs[2].TabSlots[0].ItemData = nullptr;
					CurrentLeftHandWeapon = nullptr;
					TabUpdateInfo.ReplicationCounter++;
					TabUpdateInfo.TargetTabIndex = 2;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
					//put back weapon
				}
			}
			AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner());
			if (MyChar)
			{
				UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
				if (LeftSocketInfo.Animation && AnimInst)
				{
					AnimInst->Montage_Play(LeftSocketInfo.Animation);
				}
			}
			if (UGSWeaponEquipmentComponent* weapEq = Cast<UGSWeaponEquipmentComponent>(weapon->LastInventory))
			{
				for (FGSWeaponSocketInfo& weapEqSock : weapEq->AttachmentSockets)
				{
					if (weapEqSock.SocketName == weapon->LastAttachedSocket)
					{
						weapEqSock.bIsSocketAvailable = true;
						break;
					}
				}
			}
			EquipInt->AttachActor(weapon->ActiveWeapon, LeftSocketInfo.SocketName);
		}
		if (lastWeapon)
		{
			EquipInt->AttachActor(lastWeapon->ActiveWeapon, lastWeapon->LastAttachedSocket);
		}
	}
}