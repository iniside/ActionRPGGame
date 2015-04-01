// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ActionRPGGame.h"
#include "Public/ARCharacter.h"
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
	
	Inventory->SetIsReplicated(true);
	Inventory->SetNetAddressable();

	Abilities->SetIsReplicated(true);
	Abilities->SetNetAddressable();
	//Inventory->InitializeWidgets(this);
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		
		AARCharacter* MyChar = Cast<AARCharacter>(GetPawn());
		if (Role = ROLE_Authority)
		{
			if (MyChar && !MyChar->GetAttributeComponent()->OnAttributeModifed.IsBound())
			{
				MyChar->GetAttributeComponent()->OnAttributeModifed.AddDynamic(this, &AARPlayerController::OnRecivedModifiedAttribute);
			}
		}
	}
	Super::BeginPlay();
}
//
void AARPlayerController::OnRep_Pawn()
{
	Inventory->InitializeWidgets(this);
	//Inventory->InitializeWidgets(this);
	OnPawnReplicated(GetPawn());
	
	
	if (Role < ROLE_Authority)
	{
		AARCharacter* MyChar = Cast<AARCharacter>(GetPawn());
		if (MyChar && !MyChar->GetAttributeComponent()->OnAttributeModifed.IsBound())
		{
			MyChar->GetAttributeComponent()->OnAttributeModifed.AddDynamic(this, &AARPlayerController::OnRecivedModifiedAttribute);
		}
	}
	Super::OnRep_Pawn();
}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

//
void AARPlayerController::OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn)
{
	FFCTDisplayData DisplayData;
	DisplayData.DisplayText = FText::AsNumber(AttributeModIn.ModifiedByValue);
	DisplayData.TargetLocation = AttributeModIn.TargetLocation;
	DisplayData.Tags = AttributeModIn.Tags;
	AGSHUD* gshud = Cast<AGSHUD>(GetHUD());
	if (gshud && gshud->FCTWidget)
		gshud->FCTWidget->OnReceivedData.Broadcast(DisplayData);
}

UGISInventoryBaseComponent* AARPlayerController::GetInventory()
{
	return Inventory;
}