// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "ARPlayerController.h"

AARHUD::AARHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIInventoryComponent = ObjectInitializer.CreateDefaultSubobject<UARUIInventoryComponent>(this, TEXT("UIInventoryComponent"));
}

void AARHUD::BeginPlay()
{
	Super::BeginPlay();
	if (AARPlayerController* PC = Cast<AARPlayerController>(PlayerOwner))
	{
		UIInventoryComponent->CreateInventoryView(PC);
	}
}

void AARHUD::ShowHideInventory()
{
	UIInventoryComponent->ShowHideInventory();
}