// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"
#include "GSItem.h"

#include "IGSEquipment.h"

#include "../GSCharacter.h"

#include "GSEquipmentComponent.h"

UGSEquipmentComponent::UGSEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}
void UGSEquipmentComponent::InitializeComponent()
{
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	Super::InitializeComponent();
}

void UGSEquipmentComponent::EqiupItem(class UGSItem* ItemIn)
{
	EquipedItems.Add(ItemIn);
}

void UGSEquipmentComponent::UnEqiupItem(class UGSItem* ItemIn)
{
	EquipedItems.Remove(ItemIn);
}

void UGSEquipmentComponent::SetSkeletalMesh_Implementation(USkeletalMesh* MeshIn, FName ComponentNameIn)
{
	//temporary for proofing concept.
	if (EquipInt)
	{
		EquipInt->SetSkeletalMesh(MeshIn, ComponentNameIn);
		//MyChar->SwapSkeletalMesh(MeshIn);
	}
}

void UGSEquipmentComponent::RemoveSkeletalMesh_Implementation(FName ComponentNameIn)
{
	if (EquipInt)
	{
		EquipInt->RemoveSkeletalMesh(ComponentNameIn);
		//MyChar->SwapSkeletalMesh(MeshIn);
	}
}

void UGSEquipmentComponent::AttachActorTo(AActor* ActorIn)
{
	if (EquipInt)
	{
		EquipInt->AttachActor(ActorIn, LeftBackSocketName);
	}
}