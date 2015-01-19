// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "Items/GSEquipmentComponent.h"
#include "Items/GSItemWeaponRangedInfo.h"
#include "Weapons/GSWeaponEquipmentComponent.h"
#include "Components/GSActiveActionsComponent.h"
#include "GSCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter
const FName AGSCharacter::HeadSlotComponent(TEXT("HeadSlot"));
const FName AGSCharacter::ChestSlotComponent(TEXT("ChestSlot"));
const FName AGSCharacter::HandsSlotComponent(TEXT("HandsSlot"));
const FName AGSCharacter::LegsSlotComponent(TEXT("LegsSlot"));

AGSCharacter::AGSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	Equipment = ObjectInitializer.CreateDefaultSubobject<UGSEquipmentComponent>(this, TEXT("Equipment"));
	Equipment->SetIsReplicated(true);
	Equipment->SetNetAddressable();

	WeaponsEquipment = ObjectInitializer.CreateDefaultSubobject<UGSWeaponEquipmentComponent>(this, TEXT("WeaponsEquipment"));
	WeaponsEquipment->SetIsReplicated(true);
	WeaponsEquipment->SetNetAddressable();

	RightWeaponsEquipment = ObjectInitializer.CreateDefaultSubobject<UGSWeaponEquipmentComponent>(this, TEXT("RightWeaponsEquipment"));
	RightWeaponsEquipment->SetIsReplicated(true);
	RightWeaponsEquipment->SetNetAddressable();

	ActiveActions = ObjectInitializer.CreateDefaultSubobject<UGSActiveActionsComponent>(this, TEXT("ActiveActions"));
	ActiveActions->SetIsReplicated(true);
	ActiveActions->SetNetAddressable();

	ActiveActions->LeftWeaponEquipment = WeaponsEquipment;
	ActiveActions->RightWeaponEquipment = RightWeaponsEquipment;
	ActiveActions->OnLeftWeaponChangedEvent.AddUObject(this, &AGSCharacter::SetOnLeftCurrentWeaponChanged);

	HeadComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, AGSCharacter::HeadSlotComponent);
	HeadComp->SetMasterPoseComponent(GetMesh());
	HeadComp->AttachParent = GetMesh();

	ChestComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, AGSCharacter::ChestSlotComponent);
	ChestComp->SetMasterPoseComponent(GetMesh());
	ChestComp->AttachParent = GetMesh();

	HandsComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, AGSCharacter::HandsSlotComponent);
	HandsComp->SetMasterPoseComponent(GetMesh());
	HandsComp->AttachParent = GetMesh();

	LegsComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, AGSCharacter::LegsSlotComponent);
	LegsComp->SetMasterPoseComponent(GetMesh());
	LegsComp->AttachParent = GetMesh();

	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}
void AGSCharacter::BeginPlay()
{
	Super::BeginPlay();

	Equipment->SetIsReplicated(true);
	Equipment->SetNetAddressable();

	WeaponsEquipment->SetIsReplicated(true);
	WeaponsEquipment->SetNetAddressable();

	RightWeaponsEquipment->SetIsReplicated(true);
	RightWeaponsEquipment->SetNetAddressable();

	ActiveActions->SetIsReplicated(true);
	ActiveActions->SetNetAddressable();
	DefaultLegMesh = LegsComp->SkeletalMesh;
}

void AGSCharacter::SwapSkeletalMesh(USkeletalMesh* SkeletalMesh)
{
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
}

void AGSCharacter::SetSkeletalMesh(USkeletalMesh* MeshIn, FName ComponentNameIn)
{
	FString tempString = ComponentNameIn.ToString();
	//FString LegComponentName = AGSCharacter::LegsSlotComponent.ToString();
	//if (tempString.Contains(LegComponentName))
	//	ComponentNameIn = *tempString.RightChop(LegComponentName.Len());
	
	tempString.RemoveFromStart("ItemSlot.");
	ComponentNameIn = *tempString;
	if (ComponentNameIn == AGSCharacter::LegsSlotComponent)
	{
		LegsComp->SetSkeletalMesh(MeshIn);
	}
}
void AGSCharacter::RemoveSkeletalMesh(FName ComponentNameIn)
{
	//that's excessive. 
	//if we swap items, we don't want to first swap to default mesh, and
	//then to equiped mesh.
	FString tempString = ComponentNameIn.ToString();
	//FString LegComponentName = AGSCharacter::LegsSlotComponent.ToString();
	//if (tempString.Contains(LegComponentName))
	//	ComponentNameIn = *tempString.RightChop(LegComponentName.Len());

	tempString.RemoveFromStart("ItemSlot.");
	ComponentNameIn = *tempString;
	if (ComponentNameIn == AGSCharacter::LegsSlotComponent)
	{
		LegsComp->SetSkeletalMesh(DefaultLegMesh);
	}
}
void AGSCharacter::AttachActor(AActor* ActorIn, FName SocketNameIn)
{
	if (ActorIn)
	{
		ActorIn->DetachRootComponentFromParent();
		ActorIn->AttachRootComponentTo(GetMesh(), SocketNameIn, EAttachLocation::SnapToTarget, true);
	}
}
USkeletalMeshComponent* AGSCharacter::GetSkeletalMeshComponentByName(FName NameIn)
{
	return nullptr;
}

FVector AGSCharacter::GetSocketLocation(FName SocketNameIn)
{
	return GetMesh()->GetSocketLocation(SocketNameIn);
}

USkeletalMeshComponent* AGSCharacter::GetMasterSkeletalMesh()
{
	return GetMesh();
}

FRotator AGSCharacter::GetAimOffset() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AGSCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = OwnedTags;
}

FVector AGSCharacter::GetStartLocationForCrosshair()
{
	if (ActiveActions->CurrentLeftHandWeapon)
	{
		return ActiveActions->CurrentLeftHandWeapon->GetCrosshairStartLocation();
	}
	return FVector::ZeroVector;
}
float AGSCharacter::GetCurrentWeaponSpread()
{
	return 0;
}

void AGSCharacter::SetOnLeftCurrentWeaponChanged(class UGSItemWeaponInfo* WeaponIn)
{

}