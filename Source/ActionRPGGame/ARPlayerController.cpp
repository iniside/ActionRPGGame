// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ActionRPGGame.h"
#include "Public/ARCharacter.h"
#include "GSHUD.h"
#include "ARPlayerController.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}
//void AARPlayerController::PreInitializeComponents()
//{
//	Super::PreInitializeComponents();
//}
//void AARPlayerController::BeginPlay()
//{
//	Super::BeginPlay();
//	//FCTIndicator->PCOwner = this;
//	AARCharacter* MyChar = Cast<AARCharacter>(GetPawn());
//	if (Role = ROLE_Authority)
//	{
//		if (MyChar && !MyChar->GetAttributeComponent()->OnAttributeModifed.IsBound())
//		{
//			MyChar->GetAttributeComponent()->OnAttributeModifed.AddDynamic(this, &AARPlayerController::OnRecivedModifiedAttribute);
//		}
//	}
//}
//
void AARPlayerController::OnRep_Pawn()
{
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
//
void AARPlayerController::OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn)
{
	FFCTDisplayData DisplayData;
	DisplayData.DisplayText = FString::FormatAsNumber(AttributeModIn.ModifiedByValue);
	DisplayData.TargetLocation = AttributeModIn.TargetLocation;
	DisplayData.Tags = AttributeModIn.Tags;
	AGSHUD* gshud = Cast<AGSHUD>(GetHUD());
	if (gshud)
		gshud->FCTWidget->OnReceivedData.Broadcast(DisplayData);
}