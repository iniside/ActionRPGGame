// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIComponent.h"
#include "Blueprint/UserWidget.h"

#include "ARPlayerController.h"
#include "Inventory/ARInventoryManagerWidget.h"



// Sets default values for this component's properties
UARUIComponent::UARUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	EnemyHealthBarClass = UAREnemyHealthBar::StaticClass();
	// ...
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
		if (InventoryManagerClass)
		{
			InventoryManagerWidget = CreateWidget<UARInventoryManagerWidget>(MyPC, InventoryManagerClass);
			InventoryManagerWidget->SetVisibility(ESlateVisibility::Collapsed);
			InventoryManagerWidget->AddToViewport();
		}

		if (EnemyHealthBarClass)
		{
			EnemyHealthBarWidget = CreateWidget<UAREnemyHealthBar>(MyPC, EnemyHealthBarClass);
			EnemyHealthBarWidget->SetVisibility(ESlateVisibility::Collapsed);
			EnemyHealthBarWidget->AddToViewport();
		}

		if (HUDWidgetClass)
		{
			HUDWidget = CreateWidget<UARHUDWidget>(MyPC, HUDWidgetClass);
			HUDWidget->AddToViewport();
		}

		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UIFItemContainerWidget>(MyPC, InventoryWidgetClass);
			InventoryWidget->SetInventory(MyPC->MainInventory);
			
			InventoryWidget->AddToViewport();
		}

		//DrawWidget = SNew(SHorizontalBox)
		//			+SHorizontalBox::Slot()
		//			.HAlign(EHorizontalAlignment::HAlign_Fill)
		//			.VAlign(EVerticalAlignment::VAlign_Center)
		//			[
		//				SNew(SVerticalBox)
		//				+SVerticalBox::Slot()
		//				.HAlign(EHorizontalAlignment::HAlign_Center)
		//				.VAlign(EVerticalAlignment::VAlign_Fill)
		//				[
		//					SAssignNew(CrosshairWidget2, SARDrawTestWidget)
		//				]
		//			];

		//CrosshairWidget2->Brush = &Brush;
		//GEngine->GameViewport->AddViewportWidgetContent(DrawWidget.ToSharedRef());
	}
}


// Called every frame
void UARUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARUIComponent::ShowHideInventory()
{
	if (InventoryManagerWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryManagerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InventoryManagerWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InventoryManagerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}