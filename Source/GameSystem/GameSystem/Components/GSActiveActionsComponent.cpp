 // Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "../Items/GSItemWeaponInfo.h"
#include "../Items/IGSEquipment.h"
#include "../Items/GSItemWeaponRangedInfo.h"
#include "../Weapons/GSWeaponRanged.h"
#include "../Weapons/GSWeaponEquipmentComponent.h"

#include "../Abilities/GSAbilityInfo.h"
#include "../Abilities/GSAbility.h"

#include "../GSCharacter.h" //temp!

#include "ActiveActionsWidgets/GSWeaponInfoWidget.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GSActiveActionsComponent.h"

UGSActiveActionsComponent::UGSActiveActionsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	LastRightCopiedIndex = -1;
	LastLeftCopiedIndex = -1;
	EquipingSpeedMultiplier = 1;
	UnequipingSpeedMultiplier = 1;
	WeaponSwapSpeed = 3;
	EquipingSectionIndex = 0;
	LastLeftSocket = NAME_None;
	LastRightSocket = NAME_None;
}
void UGSActiveActionsComponent::InitializeComponent()
{
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	if (LeftWeaponEquipment)
		LeftWeaponEquipment->OnItemSlotSwapped.AddDynamic(this, &UGSActiveActionsComponent::OnLeftWeaponRemoved);

	if (RightWeaponEquipment)
		RightWeaponEquipment->OnItemSlotSwapped.AddDynamic(this, &UGSActiveActionsComponent::OnRightWeaponRemoved);

	if (GetNetMode() == ENetMode::NM_Client ||
		GetNetMode() == ENetMode::NM_Standalone)
	{
		if (LeftWeaponInfoWidgetClass)
		{
			LeftWeaponInfoWidget = CreateWidget<UGSWeaponInfoWidget>(GetWorld(), LeftWeaponInfoWidgetClass);
		}

		if (RightWeaponInfoWidgetClass)
		{
			RightWeaponInfoWidget = CreateWidget<UGSWeaponInfoWidget>(GetWorld(), RightWeaponInfoWidgetClass);
		}


	}

	EquipInt = Cast<IIGSEquipment>(GetOwner());

	Super::InitializeComponent();
}

void UGSActiveActionsComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGSActiveActionsComponent, EquipWeapon);
	DOREPLIFETIME(UGSActiveActionsComponent, UnequipWeapon);
	DOREPLIFETIME_CONDITION(UGSActiveActionsComponent, CurrentLeftHandWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGSActiveActionsComponent, CurrentRightHandWeapon, COND_OwnerOnly);
}
bool UGSActiveActionsComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (LastLeftHandWeapon)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGSItemWeaponInfo*>(LastLeftHandWeapon), *Bunch, *RepFlags);
	}
	if (LastRightHandWeapon)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGSItemWeaponInfo*>(LastRightHandWeapon), *Bunch, *RepFlags);
	}
	return WroteSomething;
}
void UGSActiveActionsComponent::OnRep_CurrentLeftHandWeapon()
{
	if (LeftWeaponInfoWidget)
		LeftWeaponInfoWidget->CurrentWeapon = CurrentLeftHandWeapon;
}
void UGSActiveActionsComponent::OnRep_CurrentRightHandWeapon()
{
	if (RightWeaponInfoWidget)
		RightWeaponInfoWidget->CurrentWeapon = CurrentRightHandWeapon;
}
void UGSActiveActionsComponent::GetNextWeapon()
{

}
void UGSActiveActionsComponent::OnWeaponActive()
{

}

void UGSActiveActionsComponent::OnLeftWeaponRemoved(const FGISSlotSwapInfo& SlotSwapInfoIn)
{
	//implement weapon swapping if weapon in equipment slot changed and matches current slot.
	if (SlotSwapInfoIn.TargetSlotData == CurrentLeftHandWeapon)
	{
		float justTesting = 0;
	}
}
void UGSActiveActionsComponent::OnRightWeaponRemoved(const FGISSlotSwapInfo& SlotSwapInfoIn)
{
	if (SlotSwapInfoIn.TargetSlotData == CurrentRightHandWeapon)
	{
		float justTesting = 0;
	}
}
void UGSActiveActionsComponent::SwapWeapon(int32& LastCopiedIndex, class UGSItemWeaponInfo*& CurrentWeapon, class UGSItemWeaponInfo*& LastWeapon, class UGISInventoryBaseComponent* OtherInventory,
	int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex)
{
	int32 OtherTabSlotCount = OtherInventory->Tabs.InventoryTabs[OtherTabIndex].TabSlots.Num();
	int32 loopCounter = 0;//addtionan loop counter so we can prevent selcting empty slots if there are any.
	for (int32 TabIndex = 0; TabIndex <= OtherTabSlotCount; TabIndex++)
	{
		if (LastCopiedIndex < TabIndex)
		{
			//proceed as normal
			LastCopiedIndex++;
			if (OtherInventory->Tabs.InventoryTabs[OtherTabIndex].TabSlots[LastCopiedIndex].ItemData)
			{

				LastWeapon = Cast<UGSItemWeaponInfo>(Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData);
				//we need to delay setting up current weapon pointer, or rather activating weapon, until equiping is finished.
				CurrentWeapon = Cast<UGSItemWeaponInfo>(OtherInventory->Tabs.InventoryTabs[OtherTabIndex].TabSlots[LastCopiedIndex].ItemData);

				Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData = OtherInventory->Tabs.InventoryTabs[OtherTabIndex].TabSlots[LastCopiedIndex].ItemData;
				if (Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData)
				{
					Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->CurrentInventory = this;
					Tabs.InventoryTabs[TargetTabIndex].TabSlots[TargetSlotIndex].ItemData->LastInventory = OtherInventory;
				}
				UpdateWeapons();
				TabUpdateInfo.ReplicationCounter++;
				TabUpdateInfo.TargetTabIndex = TargetTabIndex;
				if (GetNetMode() == ENetMode::NM_Standalone)
					OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);

				if (LastCopiedIndex == (OtherTabSlotCount - 1))
					LastCopiedIndex = -1;
				return;
			}
			//if there is nothing in all slots, don't do anything.
			//just reset LastLeftCopiedIndex update weapons
			else if (LastCopiedIndex == (OtherTabSlotCount - 1))
			{
				LastCopiedIndex = -1;
			}
			else if (loopCounter == OtherTabSlotCount)
			{
				LastCopiedIndex = -1;
				return;
			}
		}
		loopCounter++;
	}
}

void UGSActiveActionsComponent::FinishSwappingWeapons(class UGSItemWeaponInfo* CurrentWeapon, class UGSItemWeaponInfo*& CurrentOppositeWeapon, class UGSItemWeaponInfo*& LastOppositeWeapon, int32 TabIndexIn)
{
	if (!CurrentWeapon)
		return;

	//and it's two handed
	if (CurrentWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		//we update last right weapon with currently equiped right hand weapon
		//and we then we null current right weapon so it no longer going to be accessible
		//as it should not be equiped to hand.
		LastOppositeWeapon = CurrentOppositeWeapon;
		Tabs.InventoryTabs[TabIndexIn].TabSlots[0].ItemData = nullptr;
		CurrentOppositeWeapon = nullptr;
	}
	/*
	If current weapon is one handed
	*/
	if (CurrentWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
	{
		if (!CurrentOppositeWeapon)
			return;
		/*
		we will check if the opposite weapon is two handed.
		if it is, it will prevent from equiping our one handed weapon.
		So we need to null it, and detach.
		*/
		if (CurrentOppositeWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
		{
			LastOppositeWeapon = CurrentOppositeWeapon;
			Tabs.InventoryTabs[TabIndexIn].TabSlots[0].ItemData = nullptr;
			CurrentOppositeWeapon = nullptr;
		}
	}
	//add functionality which will always place ActiveWeapon two handed weapon in left hand
	//so it can be activate by left mouse button. Which is what most players expect anyway.
}

void UGSActiveActionsComponent::CopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerCopyNextItemFromOtherInventoryTab(OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex, WeaponHandIn);
	}
	else
	{
		//flat out prevent changing of weapons, when ability is being used.
		if (CurrentAbility && CurrentAbility->ActiveAbility)
			if (CurrentAbility->ActiveAbility->GetIsBeingCast())
				return;

		if (!OtherIn)
			return;
		if (WeaponHandIn == EGSWeaponHand::Left)
		{
			SwapWeapon(LastLeftCopiedIndex, CurrentLeftHandWeapon, LastLeftHandWeapon, OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex);
			FinishSwappingWeapons(CurrentLeftHandWeapon, CurrentRightHandWeapon, LastRightHandWeapon, 2);
			
		}
		else if (WeaponHandIn == EGSWeaponHand::Right)
		{
			SwapWeapon(LastRightCopiedIndex, CurrentRightHandWeapon, LastRightHandWeapon, OtherIn, OtherTabIndex, TargetTabIndex, TargetSlotIndex);
			FinishSwappingWeapons(CurrentRightHandWeapon, CurrentLeftHandWeapon, LastLeftHandWeapon, 1);
		}
		if (GetNetMode() == ENetMode::NM_Standalone)
		{
			OnRep_CurrentLeftHandWeapon();
			OnRep_CurrentRightHandWeapon();
		}

		//if there is ability set current weapons when weapons change.
		if (CurrentAbility)
			CurrentAbility->GetOnSetWeaponsForAbility().Broadcast(CurrentLeftHandWeapon, CurrentRightHandWeapon);
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
void UGSActiveActionsComponent::OnRightWeaponChanged()
{
	if (EquipInt)
	{
		UpdateCurrentRightWeapon();
		UpdateLastRightWeapon();

		if (CurrentLeftHandWeapon || CurrentRightHandWeapon)
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
		}
	}
}
void UGSActiveActionsComponent::OnLeftWeaponChanged()
{
	if (EquipInt)
	{
		UpdateCurrentLeftWeapon();
		UpdateLastLeftWeapon();

		if (CurrentLeftHandWeapon || CurrentRightHandWeapon)
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
		}
	}
}

float UGSActiveActionsComponent::CalculateAttachTimeLenght()
{
	float montageLenght = LeftSocketInfo.Animation->CalculateSequenceLength();
	float multiplier = montageLenght / WeaponSwapSpeed;
	float EquipStartLen = LeftSocketInfo.Animation->GetSectionLength(EquipingSectionIndex);
	float WhenToAttach = EquipStartLen * (1 / multiplier);

	return WhenToAttach;
}

void UGSActiveActionsComponent::UpdateCurrentLeftWeapon()
{
	if (!CurrentLeftHandWeapon)
		return;

	EquipWeapon++;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_EquipWeapon();
	}

	float WhenToAttach = CalculateAttachTimeLenght();

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSActiveActionsComponent::OnLeftHandReachedWeapon);
	GetWorld()->GetTimerManager().SetTimer(TimerEquipStartHandle, del, WhenToAttach, false);
}
void UGSActiveActionsComponent::UpdateLastLeftWeapon()
{

	if (!LastLeftHandWeapon)
		return;

	UnequipWeapon++;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_UnequipWeapon();
	}
	float WhenToAttach = CalculateAttachTimeLenght();

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSActiveActionsComponent::OnLeftHandReachedSheathe);
	GetWorld()->GetTimerManager().SetTimer(TimerUnquipStartHandle, del, WhenToAttach, false);

}

void UGSActiveActionsComponent::CheckLastWeapon(class UGSItemWeaponInfo* LastWeaponIn)
{
	if (LastWeaponIn)
	{
		EquipInt->AttachActor(LastWeaponIn->GetActorToAttach(), LastWeaponIn->LastAttachedSocket);
	}
}
void UGSActiveActionsComponent::CheckCurrentWeapon(class UGSItemWeaponInfo* CurrentWeaponIn)
{
	if (CurrentWeaponIn)
	{
		EquipInt->AttachActor(CurrentWeaponIn->GetActorToAttach(), CurrentWeaponIn->LastAttachedSocket);
	}
}
void UGSActiveActionsComponent::CheckOneHand(class UGSItemWeaponInfo* LastWeapon, class UGSItemWeaponInfo* LastOppositeWeapon, class UGSItemWeaponInfo* CurrentOppositeWeapon)
{
	if (LastWeapon)
	{
		EquipInt->AttachActor(LastWeapon->GetActorToAttach(), LastWeapon->LastAttachedSocket);
	}
	//unequip weapon from right hand if it here and it's two handed.
	if (CurrentOppositeWeapon && CurrentOppositeWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		EquipInt->AttachActor(CurrentOppositeWeapon->GetActorToAttach(), CurrentOppositeWeapon->LastAttachedSocket);
	}
	if (LastOppositeWeapon && LastOppositeWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		EquipInt->AttachActor(LastOppositeWeapon->GetActorToAttach(), LastOppositeWeapon->LastAttachedSocket);
	}
}

void UGSActiveActionsComponent::CheckTwoHands(class UGSItemWeaponInfo* CurrentWeapon, class UGSItemWeaponInfo* LastLeftWeapon, class UGSItemWeaponInfo* LastRightWeapon)
{
	if (LastLeftWeapon)
	{
		EquipInt->AttachActor(LastLeftWeapon->GetActorToAttach(), LastLeftWeapon->LastAttachedSocket);
	}
	if (CurrentWeapon)
	{
		EquipInt->AttachActor(CurrentWeapon->GetActorToAttach(), CurrentWeapon->LastAttachedSocket);
	}
	if (LastRightWeapon)
	{
		EquipInt->AttachActor(LastRightWeapon->GetActorToAttach(), LastRightWeapon->LastAttachedSocket);
	}
}


void UGSActiveActionsComponent::UpdateSocket(class UGSItemWeaponInfo* WeaponIn)
{
	if (UGSWeaponEquipmentComponent* weapEq = Cast<UGSWeaponEquipmentComponent>(WeaponIn->LastInventory))
	{
		for (FGSWeaponSocketInfo& weapEqSock : weapEq->AttachmentSockets)
		{
			if (weapEqSock.SocketName == WeaponIn->LastAttachedSocket)
			{
				weapEqSock.bIsSocketAvailable = true;
				break;
			}
		}
	}
}
void UGSActiveActionsComponent::OnLeftHandReachedWeapon()
{
	if (!CurrentLeftHandWeapon)
		return;
	if (CurrentLeftHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
	{
		CheckOneHand(LastLeftHandWeapon, LastRightHandWeapon, CurrentRightHandWeapon);

		EquipInt->AttachActor(CurrentLeftHandWeapon->GetActorToAttach(), LeftSocketInfo.SocketName);
		TabUpdateInfo.ReplicationCounter++;
		TabUpdateInfo.TargetTabIndex = 1;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
	else if (CurrentLeftHandWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		CheckTwoHands(CurrentRightHandWeapon, LastLeftHandWeapon, LastRightHandWeapon);

		EquipInt->AttachActor(CurrentLeftHandWeapon->GetActorToAttach(), LeftSocketInfo.SocketName);
		TabUpdateInfo.ReplicationCounter++;
		TabUpdateInfo.TargetTabIndex = 1;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
	else if (CurrentLeftHandWeapon->GetWeaponWield() == EGSWeaponWield::Either)
	{
	}

	UpdateSocket(CurrentLeftHandWeapon);
}
void UGSActiveActionsComponent::OnLeftHandReachedSheathe()
{
	if (!LastLeftHandWeapon || LastLeftHandWeapon == CurrentLeftHandWeapon)
		return;
	EquipInt->AttachActor(LastLeftHandWeapon->GetActorToAttach(), LastLeftHandWeapon->LastAttachedSocket);

	UpdateSocket(LastLeftHandWeapon);
}

void UGSActiveActionsComponent::UpdateCurrentRightWeapon()
{
	if (!CurrentRightHandWeapon)
		return;

	EquipWeapon++;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_EquipWeapon();
	}
	float WhenToAttach = CalculateAttachTimeLenght();

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSActiveActionsComponent::OnRightHandReachedWeapon);
	GetWorld()->GetTimerManager().SetTimer(TimerRightEquipStartHandle, del, WhenToAttach, false);
}
void UGSActiveActionsComponent::UpdateLastRightWeapon()
{
	if (!LastRightHandWeapon)
		return;

	UnequipWeapon++;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_UnequipWeapon();
	}
	float WhenToAttach = CalculateAttachTimeLenght();

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSActiveActionsComponent::OnRightHandReachedSheathe);
	GetWorld()->GetTimerManager().SetTimer(TimerRightUnquipStartHandle, del, WhenToAttach, false);
}

void UGSActiveActionsComponent::OnRightHandReachedWeapon()
{
	if (!CurrentRightHandWeapon)
		return;
	if (CurrentRightHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
	{
		CheckOneHand(LastRightHandWeapon, LastLeftHandWeapon, CurrentLeftHandWeapon);

		EquipInt->AttachActor(CurrentRightHandWeapon->GetActorToAttach(), LeftSocketInfo.SocketName);
		TabUpdateInfo.ReplicationCounter++;
		TabUpdateInfo.TargetTabIndex = 1;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
	else if (CurrentRightHandWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		CheckTwoHands(CurrentLeftHandWeapon, LastLeftHandWeapon, LastRightHandWeapon);

		EquipInt->AttachActor(CurrentRightHandWeapon->GetActorToAttach(), LeftSocketInfo.SocketName);
		TabUpdateInfo.ReplicationCounter++;
		TabUpdateInfo.TargetTabIndex = 1;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
	else if (CurrentRightHandWeapon->GetWeaponWield() == EGSWeaponWield::Either)
	{
	}

	UpdateSocket(CurrentRightHandWeapon);
}
void UGSActiveActionsComponent::OnRightHandReachedSheathe()
{
	if (!LastRightHandWeapon || LastRightHandWeapon == CurrentRightHandWeapon)
		return;
	EquipInt->AttachActor(LastRightHandWeapon->GetActorToAttach(), LastRightHandWeapon->LastAttachedSocket);

	UpdateSocket(LastRightHandWeapon);
}

void UGSActiveActionsComponent::OnRep_EquipWeapon()
{
	AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner());
	if (!MyChar)
		return;

	UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
	if (!AnimInst)
		return;
	float montageLenght = LeftSocketInfo.Animation->CalculateSequenceLength();

	float multiplier = montageLenght / WeaponSwapSpeed;

	AnimInst->Montage_Play(LeftSocketInfo.Animation, multiplier);
}

void UGSActiveActionsComponent::OnRep_UnequipWeapon()
{
	AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner());
	if (!MyChar)
		return;

	UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
	if (!AnimInst)
		return;
	float montageLenght = LeftSocketInfo.Animation->CalculateSequenceLength();

	float multiplier = montageLenght / WeaponSwapSpeed;

	AnimInst->Montage_Play(LeftSocketInfo.Animation, multiplier);

}

void UGSActiveActionsComponent::UpdateWeapons()
{
	if (EquipInt)
	{
		UpdateCurrentLeftWeapon();
		UpdateLastLeftWeapon();

		UpdateCurrentRightWeapon();
		UpdateLastRightWeapon();
	}
}
void UGSActiveActionsComponent::OnWeaponEquipingEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
		return;
	Tabs.InventoryTabs[1].TabSlots[0].ItemData = CurrentLeftHandWeapon;
}

void UGSActiveActionsComponent::InputReloadWeapon(int32 TabIndex, int32 SlotIndex)
{
	UGSItemWeaponRangedInfo* weap = Cast<UGSItemWeaponRangedInfo>(Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData);
	if (weap)
	{
		weap->InputRealadWeapon();
	}
}