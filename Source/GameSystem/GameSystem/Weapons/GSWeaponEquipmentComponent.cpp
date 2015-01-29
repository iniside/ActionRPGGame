// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSCharacter.h"
#include "../Items/GSItemWeaponInfo.h"
#include "../Items/IGSEquipment.h"

#include "GSWeaponRanged.h"

#include "Widgets/GSWeaponWidget.h"
#include "Widgets/GSWeaponSetContainerWidget.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GSWeaponEquipmentComponent.h"
const FName FGSSlotNamesHelper::BackLeftSlot = "BackLeftSlot";
const FName FGSSlotNamesHelper::BackRightSlot = "BackRightSlot";
const FName FGSSlotNamesHelper::HipLeftSlot = "HipLeftSlot";
const FName FGSSlotNamesHelper::HipRightSlot = "HipRightSlot";
const FName FGSSlotNamesHelper::ShieldSlot = "ShieldSlot";
const FName FGSSlotNamesHelper::HipBackSlot = "HipBackSlot";

UGSWeaponEquipmentComponent::UGSWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	BaseEquipTime = 1;
	EquipingSectionIndex = 0;
	LastWeaponSet = -1;

	MainHandSlotIndex = 0;
	OffHandSlotIndex = 1;

	WeaponEquipState = EGSWeaponEquipState::Invalid;
}
void UGSWeaponEquipmentComponent::InitializeComponent()
{
	if (Inventory)
		Inventory->OnItemSlotSwapped.AddDynamic(this, &UGSWeaponEquipmentComponent::OnItemFromInventory);

	EquipInt = Cast<IIGSEquipment>(GetOwner());

	OnItemSlotSwapped.AddDynamic(this, &UGSWeaponEquipmentComponent::OnItemWeapEquip);
	EquipInt = Cast<IIGSEquipment>(GetOwner());

	if (LeftWeaponWidgetClass)
	{
		LeftWeaponWidget = CreateWidget<UGSWeaponWidget>(GetWorld(), LeftWeaponWidgetClass);
		if (LeftWeaponWidget)
		{
			LeftWeaponWidget->SetVisibility(LeftWeaponVisibility);
		}
	}
	if (RightWeaponWidgetClass)
	{
		RightWeaponWidget = CreateWidget<UGSWeaponWidget>(GetWorld(), RightWeaponWidgetClass);
	}
	if (WeaponSetContainerClass)
	{
		WeaponSetContainer = CreateWidget<UGSWeaponSetContainerWidget>(GetWorld(), WeaponSetContainerClass);
		if (WeaponSetContainer)
		{
			WeaponSetContainer->WeaponEquipComp = this;
			WeaponSetContainer->ItemWidgetClass = ItemWidgetClass;
			WeaponSetContainer->SlotWidgetClass = SlotWidgetClass;
			WeaponSetContainer->SetWidgetClass = SetWidgetClass;
			WeaponSetContainer->DropSlottName = WeaponSetDropSlot;
			WeaponSetContainer->SetVisibility(WeaponSetsVisibility);
			WeaponSetContainer->InitializeContainer();
		}
	}
	if (GetNetMode() == ENetMode::NM_Standalone || GetOwnerRole() < ENetRole::ROLE_Authority)
		OnWeaponSetsInitialized.ExecuteIfBound();

	Super::InitializeComponent();
}

void UGSWeaponEquipmentComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGSWeaponEquipmentComponent, MainHandWeapon);
	DOREPLIFETIME(UGSWeaponEquipmentComponent, WeaponEquipState);
	DOREPLIFETIME_CONDITION(UGSWeaponEquipmentComponent, ReplicateWeaponEquiped, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UGSWeaponEquipmentComponent, OffHandWeapon, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UGSWeaponEquipmentComponent, WeaponSets, COND_OwnerOnly);
}
void UGSWeaponEquipmentComponent::OnRep_WeaponSets()
{
	OnWeaponSetUpdated.ExecuteIfBound();
}
void UGSWeaponEquipmentComponent::OnRep_MainHandWeapon()
{
	if (LeftWeaponWidget)
		LeftWeaponWidget->CurrentWeapon = MainHandWeapon;

	AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner());
	if (!MyChar)
		return;

	UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
	if (!AnimInst)
		return;

	UAnimMontage* MontageToPlay = FindEquipMontage(MainHandWeapon);
	if (!MontageToPlay)
		return;
	float montageLenght = MontageToPlay->CalculateSequenceLength();

	float multiplier = montageLenght / BaseEquipTime;

	AnimInst->Montage_Play(MontageToPlay, multiplier);
}
void UGSWeaponEquipmentComponent::OnRep_WeaponEquiped()
{

}
void UGSWeaponEquipmentComponent::OnRep_LastLeftWeapon()
{
	AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner());
	if (!MyChar)
		return;

	UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
	if (!AnimInst)
		return;

	UAnimMontage* MontageToPlay = FindEquipMontage(LastLeftWeapon);
	if (!MontageToPlay)
		return;
	float montageLenght = MontageToPlay->CalculateSequenceLength();

	float multiplier = montageLenght / BaseEquipTime;

	AnimInst->Montage_Play(MontageToPlay, multiplier);
}
void UGSWeaponEquipmentComponent::OnRep_OffHandWeapon()
{
	OnWeaponSetUpdated.ExecuteIfBound();
}

bool UGSWeaponEquipmentComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (MainHandWeapon)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGSItemWeaponInfo*>(MainHandWeapon), *Bunch, *RepFlags);
	}
	if (OffHandWeapon)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGSItemWeaponInfo*>(OffHandWeapon), *Bunch, *RepFlags);
	}

	return WroteSomething;
}
void UGSWeaponEquipmentComponent::CheckWeaponSet(int32 SetIndex)
{
	int32 SlotCounter = 0;
	for (FGSWeaponSetSlot& setSlot : WeaponSets[SetIndex].Weapons)
	{
		if (setSlot.WeaponID >= 0 && setSlot.WeaponTabID >= 0)
		{
			if (WeaponSets[SetIndex].bIsSetEquiped)
			{
				if (SlotCounter == 0)
				{
					LastLeftWeapon = MainHandWeapon;
					LastLeftWeapon->InputReleased();
					MainHandWeapon = nullptr;
				}
				else if (SlotCounter == 1)
				{

				}
			}
			//let's check if this weapon can share set with other weapon. Refactor into separate function later!
			UGSItemWeaponInfo* weap = CastChecked<UGSItemWeaponInfo>(Tabs.InventoryTabs[setSlot.WeaponTabID].TabSlots[setSlot.WeaponID].ItemData);

			if (weap->GetWeaponWield() == EGSWeaponWield::TwoHands)
			{
				//remove weapon from set
				setSlot.WeaponID = INDEX_NONE;
				setSlot.WeaponTabID = INDEX_NONE;
				//need to cover more complex case. What  if weapon is currently in hand ?
			}
		}
		SlotCounter++;
	}
}
void UGSWeaponEquipmentComponent::AddWeaponToSlotInSet(int32 WeaponIDIn, int32 WeaponTabIDIn, int32 SlotIndex,
	int32 SetIndex)
{
	UGSItemWeaponInfo* weap = Cast<UGSItemWeaponInfo>(Tabs.InventoryTabs[WeaponTabIDIn].TabSlots[WeaponIDIn].ItemData);
	if (weap->GetWeaponType() == EGSWeaponType::MainHand || weap->GetWeaponWield() == EGSWeaponWield::TwoHands)
	{
		WeaponSets[SetIndex].Weapons[MainHandSlotIndex].WeaponID = WeaponIDIn;
		WeaponSets[SetIndex].Weapons[MainHandSlotIndex].WeaponTabID = WeaponTabIDIn;
	}
	else if (weap->GetWeaponType() == EGSWeaponType::OffHand)
	{
		WeaponSets[SetIndex].Weapons[OffHandSlotIndex].WeaponID = WeaponIDIn;
		WeaponSets[SetIndex].Weapons[OffHandSlotIndex].WeaponTabID = WeaponTabIDIn;
	}
	else if (weap->GetWeaponType() == EGSWeaponType::Either)
	{
		WeaponSets[SetIndex].Weapons[SlotIndex].WeaponID = WeaponIDIn;
		WeaponSets[SetIndex].Weapons[SlotIndex].WeaponTabID = WeaponTabIDIn;
	}
}
void UGSWeaponEquipmentComponent::AddWeaponToSet(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddWeaponToSet(SlotIndex, TabIndex, TargetSlot);
	}
	else
	{
		CheckWeaponSet(TargetSlot.SetIndex);
		int32 OldWeaponID = WeaponSets[TargetSlot.SetIndex].Weapons[TargetSlot.SlotIndex].WeaponID;
		int32 OldWeaponTabID = WeaponSets[TargetSlot.SetIndex].Weapons[TargetSlot.SlotIndex].WeaponTabID;
		if (WeaponSets[TargetSlot.SetIndex].bIsSetEquiped)
		{
			MainHandWeapon = Cast<UGSItemWeaponInfo>(Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData);
			UpdateWeapons();
		}
		AddWeaponToSlotInSet(SlotIndex, TabIndex, TargetSlot.SlotIndex, TargetSlot.SetIndex);
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnWeaponSetUpdated.ExecuteIfBound();
	}
}
void UGSWeaponEquipmentComponent::ServerAddWeaponToSet_Implementation(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot)
{
	AddWeaponToSet(SlotIndex, TabIndex, TargetSlot);
}
bool UGSWeaponEquipmentComponent::ServerAddWeaponToSet_Validate(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot)
{
	return true;
}

void UGSWeaponEquipmentComponent::AddWeaponToSet(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddWeaponToSetFourParams(SlotIndex, TabIndex, TargetSlot, LastSlot);
	}
	else
	{
		FGSWeaponSetSlot TargetSlotCopy = WeaponSets[TargetSlot.SetIndex].Weapons[TargetSlot.SlotIndex];

		CheckWeaponSet(TargetSlot.SetIndex);
		CheckWeaponSet(LastSlot.SetIndex);

		AddWeaponToSlotInSet(SlotIndex, TabIndex, TargetSlot.SlotIndex, TargetSlot.SetIndex);
		AddWeaponToSlotInSet(TargetSlotCopy.WeaponID, TargetSlotCopy.WeaponTabID, LastSlot.SlotIndex, LastSlot.SetIndex);
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnWeaponSetUpdated.ExecuteIfBound();
	}

}
void UGSWeaponEquipmentComponent::ServerAddWeaponToSetFourParams_Implementation(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot)
{
	AddWeaponToSet(SlotIndex, TabIndex, TargetSlot, LastSlot);
}
bool UGSWeaponEquipmentComponent::ServerAddWeaponToSetFourParams_Validate(int32 SlotIndex, int32 TabIndex,
	const FGSWeaponSetSlot& TargetSlot, const FGSWeaponSetSlot& LastSlot)
{
	return true;
}
void UGSWeaponEquipmentComponent::OnItemFromInventory(const FGISSlotSwapInfo& SlotSwapInfoIn)
{

}
void UGSWeaponEquipmentComponent::OnItemWeapEquip(const FGISSlotSwapInfo& SlotSwapInfoIn)
{

}

bool UGSWeaponEquipmentComponent::CheckIfCanAddItemToSlot(class UGISItemData* TargetDataIn, int32 TargetTabIndex, int32 TargetSlotIndex,
	class UGISItemData* LastDataIn)
{
	UGSItemWeaponInfo* tarWeap = Cast<UGSItemWeaponInfo>(TargetDataIn);
	if (!tarWeap)
		return false;
	
	//look for any potentional free socket.
	//which essentialy equal free weapon slot.
	bool bFoundFreeSocket = false;
	FName FoundSocketName;
	const TArray<FGSWeaponSocketInfo>& weapSockets = tarWeap->GetPossibleWeaponSockets();
	int32 weapSockNum = weapSockets.Num();

	/*
		Sockets act as limiting factor for how much weapons can be equiped.
		And by extenstion how much weapons of certain type. Because each weapon
		have individually specified to which sockets in can be attached.
		If we passed here, and all sockets are locked
		we should find first matching socket, unequipd weapon from it
		and equipd new one.
		*/
	for (auto It = AttachmentSockets.CreateIterator(); It; ++It)
	{
		//first we look for socket which is associated with this particular slot.
		if (It->InventorySlotName == Tabs.InventoryTabs[TargetTabIndex].TabName)
		{
			//we check if this socket is free.
			//notably if there is weapon it's not free of course..
			if (It->bIsSocketAvailable) 
			{
				//now we check if this weapon can be attached to this socket.
				for (int32 idx = 0; idx < weapSockNum; idx++)
				{
					if (It->SocketName == weapSockets[idx].SocketName)
					{
						bFoundFreeSocket = true;
						SocketToAttach = It->SocketName;
						It->bIsSocketAvailable = false;
						return bFoundFreeSocket;
					}
				}
			}
			else
			{
				//here we should attempy unequip weapon and move it back to inventory.
				//right now we won't do anything.
			}
		}
	}
	return bFoundFreeSocket;
}

void UGSWeaponEquipmentComponent::UnequipWeapon()
{

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
/////////////////////////
////////// Input Begin
void UGSWeaponEquipmentComponent::InputLeftWeaponPressed()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerInputLeftWeaponPressed();
		if (MainHandWeapon)
			MainHandWeapon->InputPressed();
	}
	else
	{
		if (MainHandWeapon)
			MainHandWeapon->InputPressed();
	}
}
void UGSWeaponEquipmentComponent::ServerInputLeftWeaponPressed_Implementation()
{
	InputLeftWeaponPressed();
}
bool UGSWeaponEquipmentComponent::ServerInputLeftWeaponPressed_Validate()
{
	return true;
}

void UGSWeaponEquipmentComponent::InputLeftWeaponReleased()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerInputLeftWeaponReleased();
		if (MainHandWeapon)
			MainHandWeapon->InputReleased();
	}
	else
	{
		if (MainHandWeapon)
			MainHandWeapon->InputReleased();
	}
}
void UGSWeaponEquipmentComponent::ServerInputLeftWeaponReleased_Implementation()
{
	InputLeftWeaponReleased();
}
bool UGSWeaponEquipmentComponent::ServerInputLeftWeaponReleased_Validate()
{
	return true;
}

void UGSWeaponEquipmentComponent::InputRightWeaponPressed()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerInputRightWeaponPressed();
		if (OffHandWeapon)
			OffHandWeapon->InputPressed();
	}
	else
	{
		if (OffHandWeapon)
			OffHandWeapon->InputPressed();
	}
}
void UGSWeaponEquipmentComponent::ServerInputRightWeaponPressed_Implementation()
{
	InputRightWeaponPressed();
}
bool UGSWeaponEquipmentComponent::ServerInputRightWeaponPressed_Validate()
{
	return true;
}

void UGSWeaponEquipmentComponent::InputRightWeaponReleased()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerInputRightWeaponReleased();
		if (OffHandWeapon)
			OffHandWeapon->InputReleased();
	}
	else
	{
		if (OffHandWeapon)
			OffHandWeapon->InputReleased();
	}
}
void UGSWeaponEquipmentComponent::ServerInputRightWeaponReleased_Implementation()
{
	InputRightWeaponReleased();
}
bool UGSWeaponEquipmentComponent::ServerInputRightWeaponReleased_Validate()
{
	return true;
}
////////// Input End
////////////////////////////

void UGSWeaponEquipmentComponent::UpdateWeapons()
{
	if (!EquipInt)
		return;
	UpdateMainHandWeapon();
	UpdateLastLeftWeapon();
}
UAnimMontage* UGSWeaponEquipmentComponent::FindEquipMontage(class UGSItemWeaponInfo* WeaponIn)
{
	UAnimMontage* MontageToPlay = nullptr;
	for (const FGSEquipSocketInfo& socket : AttachmentSockets)
	{
		if (socket.SocketName == WeaponIn->LastAttachedSocket)
		{
			MontageToPlay = socket.Animation;
			break;
		}
	}
	return MontageToPlay;
}
float UGSWeaponEquipmentComponent::CalculateAttachTimeLenght(class UGSItemWeaponInfo* WeaponIn)
{
	float montagelenght = 1;
	float equipstartlen = .5;
	UAnimMontage* Montage = FindEquipMontage(WeaponIn);
	if (Montage)
	{
		montagelenght = Montage->CalculateSequenceLength();
		Montage->GetSectionLength(EquipingSectionIndex);
	}
	float multiplier = montagelenght / BaseEquipTime;
	float whentoattach = equipstartlen * (1 / multiplier);
	return whentoattach; //WhenToAttach;
}
void UGSWeaponEquipmentComponent::UpdateMainHandWeapon()
{
	if (!MainHandWeapon)
		return;
	if (CheckIfStandloneOrAuthority())
	{
		OnRep_MainHandWeapon();
	}
	ReplicateWeaponEquiped++;

	float WhenToAttach = CalculateAttachTimeLenght(MainHandWeapon);

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSWeaponEquipmentComponent::OnLeftHandReachedWeapon);
	GetWorld()->GetTimerManager().SetTimer(TimerEquipStartHandle, del, WhenToAttach, false);
}
void UGSWeaponEquipmentComponent::UpdateLastLeftWeapon()
{
	if (!LastLeftWeapon)
		return;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		//OnRep_LastLeftWeapon();
	}
	ReplicateWeaponEquiped++;
	float WhenToAttach = CalculateAttachTimeLenght(LastLeftWeapon);

	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGSWeaponEquipmentComponent::OnLeftHandReachedSheathe);
	GetWorld()->GetTimerManager().SetTimer(TimerUnquipStartHandle, del, WhenToAttach, false);
}

void UGSWeaponEquipmentComponent::OnLeftHandReachedWeapon()
{
	if (!MainHandWeapon)
		return;

	//if (LastLeftWeapon)
	//{
	//	EquipInt->AttachActor(LastLeftWeapon->GetActorToAttach(), LastLeftWeapon->LastAttachedSocket);
	//}
	EquipInt->AttachActor(MainHandWeapon->GetActorToAttach(), MainHandSocketInfo.SocketName);
}
void UGSWeaponEquipmentComponent::OnLeftHandReachedSheathe()
{
	if (LastLeftWeapon)
	{
		EquipInt->AttachActor(LastLeftWeapon->GetActorToAttach(), LastLeftWeapon->LastAttachedSocket);
	}
}
void UGSWeaponEquipmentComponent::SwapWeaponSet()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerSwapWeaponSet();
		SimulateSwapWeaponSet();
	}
	else
	{
		SimulateSwapWeaponSet();
	}
}
void UGSWeaponEquipmentComponent::SimulateSwapWeaponSet()
{
	int32 SetCount = WeaponSets.Num();
	for (int32 SetIndex = 0; SetIndex < SetCount; SetIndex++)
	{
		if (LastWeaponSet < SetIndex)
		{
			if (WeaponSets.IsValidIndex(LastWeaponSet))
				WeaponSets[LastWeaponSet].bIsSetEquiped = false;

			LastWeaponSet++;
			int32 LeftWeaponID = WeaponSets[SetIndex].Weapons[MainHandSlotIndex].WeaponID;
			int32 LeftWeaponTabID = WeaponSets[SetIndex].Weapons[MainHandSlotIndex].WeaponTabID;
			int32 RightWeaponID = WeaponSets[SetIndex].Weapons[OffHandSlotIndex].WeaponID;
			int32 RightWeaponTabID = WeaponSets[SetIndex].Weapons[OffHandSlotIndex].WeaponTabID;

			LastLeftWeapon = MainHandWeapon;
			LastRightWeapon = OffHandWeapon;
			if (Tabs.InventoryTabs.IsValidIndex(LeftWeaponTabID)
				&& Tabs.InventoryTabs[LeftWeaponTabID].TabSlots.IsValidIndex(LeftWeaponID))
			{
				MainHandWeapon = Cast<UGSItemWeaponInfo>(Tabs.InventoryTabs[LeftWeaponTabID].TabSlots[LeftWeaponID].ItemData);
			}
			else
			{
				MainHandWeapon = nullptr;
			}
			if (Tabs.InventoryTabs.IsValidIndex(RightWeaponTabID)
				&& Tabs.InventoryTabs[RightWeaponTabID].TabSlots.IsValidIndex(RightWeaponID))
			{
				OffHandWeapon = Cast<UGSItemWeaponInfo>(Tabs.InventoryTabs[RightWeaponTabID].TabSlots[RightWeaponID].ItemData);
			}
			else
			{
				OffHandWeapon = nullptr;
			}
			WeaponSets[SetIndex].bIsSetEquiped = true;
			UpdateWeapons();

			if (MainHandWeapon && OffHandWeapon
				&& OffHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand
				&& MainHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
			{
				WeaponEquipState = EGSWeaponEquipState::DualWield;

				MainHandWeapon->CurrentHand = EGSWeaponHand::Right;
				OffHandWeapon->CurrentHand = EGSWeaponHand::Left;
			}
			else if (MainHandWeapon && !OffHandWeapon
				&& MainHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
			{
				WeaponEquipState = EGSWeaponEquipState::MainHand;
				MainHandWeapon->CurrentHand = EGSWeaponHand::Right;
			}
			else if (OffHandWeapon && !MainHandWeapon
				&& OffHandWeapon->GetWeaponWield() == EGSWeaponWield::OneHand)
			{
				WeaponEquipState = EGSWeaponEquipState::OffHand;
				OffHandWeapon->CurrentHand = EGSWeaponHand::Left;
			}
			else if (MainHandWeapon &&
				MainHandWeapon->GetWeaponWield() == EGSWeaponWield::TwoHands) //two handed weapons will always end up in main hand slot.
			{
				WeaponEquipState = EGSWeaponEquipState::MainHand;
				MainHandWeapon->CurrentHand = EGSWeaponHand::BothHands;
			}
			else if (!MainHandWeapon && !OffHandWeapon)
			{
				WeaponEquipState = EGSWeaponEquipState::Invalid;
			}
			if (LeftWeaponWidget)
				LeftWeaponWidget->CurrentWeapon = MainHandWeapon;

			if (LastWeaponSet == (SetCount - 1))
				LastWeaponSet = -1;

			return;
		}
	}
}
void UGSWeaponEquipmentComponent::ServerSwapWeaponSet_Implementation()
{
	SwapWeaponSet();
}
bool UGSWeaponEquipmentComponent::ServerSwapWeaponSet_Validate()
{
	return true;
}

UAnimSequence* UGSWeaponEquipmentComponent::GetMainHandAnimSequence() const
{
	return GetWeaponAnimSequence(MainHandWeapon);
}
UAnimSequence* UGSWeaponEquipmentComponent::GetOffHandAnimSequence() const
{
	return GetWeaponAnimSequence(OffHandWeapon);
}
UAnimSequence* UGSWeaponEquipmentComponent::GetWeaponAnimSequence(class UGSItemWeaponInfo* WeaponIn) const
{
	if (!WeaponIn)
		return nullptr;

	if (bIsInCombatStance)
	{
		return WeaponIn->GetCombatAnimation();
	}
	else
	{
		return WeaponIn->GetIdleAnimation();
	}

	return nullptr;
}
bool UGSWeaponEquipmentComponent::CheckIfStandloneOrAuthority()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
		return true;
	return false;
}