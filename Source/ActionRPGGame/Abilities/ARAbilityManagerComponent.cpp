// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityManagerComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Layout/Visibility.h"

#include "DWBPFunctionLibrary.h"
#include "SDraggableWindowWidget.h"

#include "../UI/Abilities/ARAbilityManagerWidget.h"

// Sets default values for this component's properties
UARAbilityManagerComponent::UARAbilityManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
		// ...
		if (ManagerWidgetClass)
		{
			ManagerWidget = CreateWidget<UARAbilityManagerWidget>(OwnerPC, ManagerWidgetClass);
			ManagerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ManagerWindowHandle = UDWBPFunctionLibrary::CreateWindowWithContent(ManagerWidget, "Ability Manager");
			ManagerWindowHandle.Window.Pin()->SetVisibility(EVisibility::Collapsed);
			//ManagerWidget->AddToViewport();
		}
	}
}


// Called every frame
void UARAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARAbilityManagerComponent::ShowHideAbilityManager()
{
	if (!ManagerWidget)
		return;

	EVisibility Visibility = ManagerWindowHandle.Window.Pin()->GetVisibility();
	
	if (Visibility == EVisibility::Collapsed)
	{
		ManagerWindowHandle.Window.Pin()->SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	else if (Visibility == EVisibility::SelfHitTestInvisible)
	{
		ManagerWindowHandle.Window.Pin()->SetVisibility(EVisibility::Collapsed);
	}
}