// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIComponent.h"
#include "Blueprint/UserWidget.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"

#include "Inventory/Weapons/ARWeaponContainerWidget.h"
#include "Inventory/Weapons/ARItemWeaponWidget.h"
#include "Inventory/ARInventoryScreenWidget.h"



// Sets default values for this component's properties
UARUIComponent::UARUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoRegister = true;
	bWantsInitializeComponent = true;
	// ...
}

void UARUIComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
// Called when the game starts
void UARUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwner());
		// ...
		if (CrosshairClass)
		{
			CrosshairWidget = CreateWidget<UUserWidget>(MyPC, CrosshairClass);
			CrosshairWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			CrosshairWidget->AddToViewport();
		}

		if (HUDWidgetClass)
		{
			HUDWidget = CreateWidget<UARHUDWidget>(MyPC, HUDWidgetClass);
			HUDWidget->AddToViewport();
		}
	}
}


// Called every frame
void UARUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARUIComponent::InitializeInventory()
{

}
