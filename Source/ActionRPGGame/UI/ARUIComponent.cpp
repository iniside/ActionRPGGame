// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIComponent.h"
#include "Blueprint/UserWidget.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"

#include "Inventory/ARInventoryManagerWidget.h"
#include "Inventory/ARWeaponContainerWidget.h"
#include "Inventory/ARItemWeaponWidget.h"
#include "Inventory/ARInventoryScreenWidget.h"

// Sets default values for this component's properties
UARUIComponent::UARUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	EnemyHealthBarClass = UAREnemyHealthBar::StaticClass();
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

		AARCharacter* Character = Cast<AARCharacter>(MyPC->GetPawn());
		if (WeaponInventoryWidgetClass && Character)
		{
			InventoryScreenWidget->UI = this;
			InventoryScreenWidget->PC = MyPC;
			InventoryScreenWidget->Inventory = MyPC->MainInventory;

			UARWeaponInventoryComponent* WeapInv = Character->WeaponInventory;

			for (FIFItemData& Item : WeapInv->GetInventory().GetItems())
			{
				switch (Item.GetLocalIndex())
				{
				case 0:
				{
					InventoryScreenWidget->RightWeaponWidget->Inventory = MyPC->MainInventory;
					InventoryScreenWidget->RightWeaponWidget->WeaponInventory = WeapInv;
					InventoryScreenWidget->RightWeaponWidget->UI = this;
					InventoryScreenWidget->RightWeaponWidget->InventoryWidget = InventoryScreenWidget;

					InventoryScreenWidget->RightWeaponWidget->LocalIndex = Item.GetLocalIndex();
					InventoryScreenWidget->RightWeaponWidget->NetIndex = Item.GetNetIndex();

					FIFOnItemChangedEvent Event = FIFOnItemChangedEvent::CreateUObject(InventoryScreenWidget->RightWeaponWidget, &UIFItemWidget::OnItemChanged);
					Item.SetOnItemChanged(Event);
				}
				case 1:
				{
					InventoryScreenWidget->LeftWeaponWidget->Inventory = MyPC->MainInventory;
					InventoryScreenWidget->LeftWeaponWidget->WeaponInventory = WeapInv;
					InventoryScreenWidget->LeftWeaponWidget->UI = this;
					InventoryScreenWidget->LeftWeaponWidget->InventoryWidget = InventoryScreenWidget;

					InventoryScreenWidget->LeftWeaponWidget->LocalIndex = Item.GetLocalIndex();
					InventoryScreenWidget->LeftWeaponWidget->NetIndex = Item.GetNetIndex();

					FIFOnItemChangedEvent Event = FIFOnItemChangedEvent::CreateUObject(InventoryScreenWidget->LeftWeaponWidget, &UIFItemWidget::OnItemChanged);
					Item.SetOnItemChanged(Event);
				}
				case 2:
				{
					InventoryScreenWidget->SideWeaponWidget->Inventory = MyPC->MainInventory;
					InventoryScreenWidget->SideWeaponWidget->WeaponInventory = WeapInv;
					InventoryScreenWidget->SideWeaponWidget->UI = this;
					InventoryScreenWidget->SideWeaponWidget->InventoryWidget = InventoryScreenWidget;

					InventoryScreenWidget->SideWeaponWidget->LocalIndex = Item.GetLocalIndex();
					InventoryScreenWidget->SideWeaponWidget->NetIndex = Item.GetNetIndex();

					FIFOnItemChangedEvent Event = FIFOnItemChangedEvent::CreateUObject(InventoryScreenWidget->SideWeaponWidget, &UIFItemWidget::OnItemChanged);
					Item.SetOnItemChanged(Event);
				}
				case 3:
				{
					InventoryScreenWidget->BottomBackWeaponWidget->Inventory = MyPC->MainInventory;
					InventoryScreenWidget->BottomBackWeaponWidget->WeaponInventory = WeapInv;
					InventoryScreenWidget->BottomBackWeaponWidget->UI = this;
					InventoryScreenWidget->BottomBackWeaponWidget->InventoryWidget = InventoryScreenWidget;

					InventoryScreenWidget->BottomBackWeaponWidget->LocalIndex = Item.GetLocalIndex();
					InventoryScreenWidget->BottomBackWeaponWidget->NetIndex = Item.GetNetIndex();

					FIFOnItemChangedEvent Event = FIFOnItemChangedEvent::CreateUObject(InventoryScreenWidget->BottomBackWeaponWidget, &UIFItemWidget::OnItemChanged);
					Item.SetOnItemChanged(Event);
				}
				}
			}
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
void UARUIComponent::InitializeWeaponInventory()
{
	//if (GetOwner()->GetNetMode() == ENetMode::NM_Client
	//	|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	//{
	//	AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwner());
	//	AARCharacter* Character = Cast<AARCharacter>(MyPC->GetPawn());
	//	if (WeaponInventoryWidgetClass && Character)
	//	{
	//		InventoryScreenWidget->WeaponInventoryWidget = CreateWidget<UARWeaponContainerWidget>(MyPC, WeaponInventoryWidgetClass);
	//		
	//		InventoryScreenWidget->UI = this;
	//		InventoryScreenWidget->PC = MyPC;
	//		InventoryScreenWidget->Inventory = MyPC->MainInventory;
	//		InventoryScreenWidget->WeaponInventoryWidget->SetInventory(Character->WeaponInventory);
	//		InventoryScreenWidget->WeaponInventoryWidget->CreateInventory();
	//		InventoryScreenWidget->WeaponInventoryWidget->InitializeWeaponItems(this);
	//		InventoryScreenWidget->WeaponInventoryWidget->AddToViewport();
	//		
	//	}
	//}
}
void UARUIComponent::ShowHideInventory()
{
	if (InventoryScreenWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryScreenWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InventoryScreenWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InventoryScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}