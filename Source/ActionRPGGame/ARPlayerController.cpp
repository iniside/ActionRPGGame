// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "ARUIAbilityManagerComponent.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	UIAbilityManagerComponent = ObjectInitializer.CreateDefaultSubobject<UARUIAbilityManagerComponent>(this, "UIAbilityManagerComponent");
}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("SwitchAbilitySet", IE_Pressed, this, &AARPlayerController::InputSwitchAbilitySet);
}

void AARPlayerController::InputSwitchAbilitySet()
{
	UIAbilityManagerComponent->SwitchSet();
}