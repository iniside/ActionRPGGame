// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "Items/GSEquipmentComponent.h"

#include "GSCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

AGSCharacter::AGSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	Equipment = ObjectInitializer.CreateDefaultSubobject<UGSEquipmentComponent>(this, TEXT("Equipment"));
	Equipment->SetIsReplicated(true);
	Equipment->SetNetAddressable();
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}
void AGSCharacter::BeginPlay()
{
	Super::BeginPlay();

	Equipment->SetIsReplicated(true);
	Equipment->SetNetAddressable();
}

void AGSCharacter::SwapSkeletalMesh(USkeletalMesh* SkeletalMesh)
{
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
}