// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ActionRPGGame.h"
#include "Public/ARCharacter.h"
#include "GSHUD.h"
#include "Abilities/GSAbilitiesComponent.h"
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
	Super::BeginPlay();
	Inventory->SetIsReplicated(true);
	Inventory->SetNetAddressable();

	Abilities->SetIsReplicated(true);
	Abilities->SetNetAddressable();

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
}
//
void AARPlayerController::OnRep_Pawn()
{
	OnPawnReplicated(GetPawn());
	Super::OnRep_Pawn();
	if (Role < ROLE_Authority)
	{
		AARCharacter* MyChar = Cast<AARCharacter>(GetPawn());
		if (MyChar && !MyChar->GetAttributeComponent()->OnAttributeModifed.IsBound())
		{
			MyChar->GetAttributeComponent()->OnAttributeModifed.AddDynamic(this, &AARPlayerController::OnRecivedModifiedAttribute);
		}
	}
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