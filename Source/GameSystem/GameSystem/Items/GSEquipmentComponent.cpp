// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "../GSCharacter.h"

#include "GSEquipmentComponent.h"

UGSEquipmentComponent::UGSEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UGSEquipmentComponent::EquiptItem(USkeletalMesh* MeshIn)
{
	//temporary for proofing concept.
	if (AGSCharacter* MyChar = Cast<AGSCharacter>(GetOwner()))
	{
		MyChar->SwapSkeletalMesh(MeshIn);
	}
}