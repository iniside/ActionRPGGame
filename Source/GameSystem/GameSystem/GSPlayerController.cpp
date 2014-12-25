// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameSystem.h"

#include "GSPlayerController.h"

AGSPlayerController::AGSPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCTIndicator = ObjectInitializer.CreateDefaultSubobject<UFCTFloatingTextComponent>(this, TEXT("FCTIndicator"));
}

void AGSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FCTIndicator->Activate();
	FCTIndicator->RegisterComponent();
	FCTIndicator->PCOwner = this;
}

//
//void AGSPlayerController::OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn)
//{
//	FFCTDisplayData DisplayData;
//	DisplayData.DisplayText = FString::FormatAsNumber(AttributeModIn.ModifiedByValue);
//	DisplayData.Target = AttributeModIn.Target;
//	DisplayData.Tags = AttributeModIn.Tags;
//	FCTIndicator->OnReceivedData.Broadcast(DisplayData);
//}