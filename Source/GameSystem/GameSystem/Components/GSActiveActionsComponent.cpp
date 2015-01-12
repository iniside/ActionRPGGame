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
	LastItemCopiedIndex = -1;
	LastCopiedTabIndex = -1;
}
void UGSActiveActionsComponent::InitializeComponent()
{
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	Super::InitializeComponent();
}
void UGSActiveActionsComponent::CopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerCopyNextItemFromOtherInventoryTab(OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex);
	}
	else
	{
		if (OtherIn)
		{
			int32 OtherTabSlotCount = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots.Num();

			for (int32 TabIndex = 0; TabIndex < OtherTabSlotCount; TabIndex++)
			{
				if (LastItemCopiedIndex < TabIndex)
				{
					LastItemCopiedIndex++;
					LastCopiedTabIndex = OtherTabIndex;
					UGISItemData* LastData = Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData;
					Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData = OtherIn->Tabs.InventoryTabs[OtherTabIndex].TabSlots[TabIndex].ItemData;
					if (Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData)
					{
						Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->CurrentInventory = this;
						Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->LastInventory = OtherIn;
					}
					
					OnActionCopiedFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData, LastData);
	
					TabUpdateInfo.ReplicationCounter++;
					TabUpdateInfo.TargetTabIndex = TargetTabIndex;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);

					if (LastItemCopiedIndex == (OtherTabSlotCount - 1))
						LastItemCopiedIndex = -1;

					return;
				}
			}
		}
	}
}
void UGSActiveActionsComponent::ServerCopyNextItemFromOtherInventoryTab_Implementation(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex)
{
	CopyNextItemFromOtherInventoryTab(OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex);
}
bool UGSActiveActionsComponent::ServerCopyNextItemFromOtherInventoryTab_Validate(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex)
{
	return true;
}

void UGSActiveActionsComponent::OnActionCopiedFromOtherInventoryTab(class UGISItemData* DataIn, class UGISItemData* LastDataIn)
{
	if (EquipInt)
	{
		if (UGSItemWeaponInfo* weapon = Cast<UGSItemWeaponInfo>(DataIn))
		{
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
					}
				}
			}
			EquipInt->AttachActor(weapon->ActiveWeapon, LeftSocketInfo.SocketName);
		}
		if (UGSItemWeaponInfo* weapon = Cast<UGSItemWeaponInfo>(LastDataIn))
		{
			EquipInt->AttachActor(weapon->ActiveWeapon, weapon->LastAttachedSocket);
		}
	}
}