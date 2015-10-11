// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ActionRPGGame.h"
#include "ARCharacter.h"
#include "GSHUD.h"
#include "Abilities/GSAbilitiesComponent.h"
#include "GISInventoryBaseComponent.h"
#include "ARPlayerController.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Inventory = ObjectInitializer.CreateDefaultSubobject<UGISInventoryBaseComponent>(this, TEXT("Inventory"));
	Inventory->SetIsReplicated(true);
	Inventory->SetNetAddressable();


	Abilities = ObjectInitializer.CreateDefaultSubobject<UGSAbilitiesComponent>(this, TEXT("Abilities"));
	Abilities->SetIsReplicated(true);
	Abilities->SetNetAddressable();
}

void AARPlayerController::BeginPlay()
{
	Inventory->SetPCOwner(this);
	Inventory->SetIsReplicated(true);
	Inventory->SetNetAddressable();

	Abilities->SetIsReplicated(true);
	Abilities->SetNetAddressable();
	//Inventory->InitializeWidgets(this);

	Super::BeginPlay();
}
//
void AARPlayerController::OnRep_Pawn()
{
	Inventory->InitializeWidgets(this);
	//Inventory->InitializeWidgets(this);
	OnPawnReplicated(GetPawn());
	
	Super::OnRep_Pawn();
}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

//
void AARPlayerController::OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn)
{

}

UGISInventoryBaseComponent* AARPlayerController::GetInventory()
{
	return Inventory;
}