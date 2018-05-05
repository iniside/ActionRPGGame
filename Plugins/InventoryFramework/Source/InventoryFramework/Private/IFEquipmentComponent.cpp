// Fill out your copyright notice in the Description page of Project Settings.

#include "IFEquipmentComponent.h"
#include "IFInventoryComponent.h"

// Sets default values for this component's properties
UIFEquipmentComponent::UIFEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UIFEquipmentComponent::BeginPlay()
{
	uint8 Counter = 0;
	for (uint8 Idx = 0; Idx < MaxSlots; Idx++)
	{
		FIFItemData NewItem;
		NewItem.Item = nullptr;
		NewItem.Index = Idx;
		Counter++;
		EquipmentItems.Add(NewItem);
	}
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UIFEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UIFEquipmentComponent::AddItemFromInventory(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		UIFItemBase* Item = Source->GetItem(SourceIndex);
		OnClientPreItemAdded(Item, EquipmentIndex);
		ServerAddItemFromInventory(Source, SourceIndex, EquipmentIndex);
	}
	else
	{
		UIFItemBase* Item = Source->GetItem(SourceIndex);
		if (!Item)
			return;

		EquipmentItems[EquipmentIndex].Item = DuplicateObject<UIFItemBase>(Item, this);
		
		OnItemAdded(EquipmentItems[EquipmentIndex].Item, EquipmentIndex);
		EquipmentItems[EquipmentIndex].Item->OnServerItemAddedEquipment(EquipmentItems[EquipmentIndex].Index);

		OnItemAddedEvent.Broadcast(EquipmentIndex, EquipmentIndex, EquipmentItems[EquipmentIndex].Item);
		
	}
}
void UIFEquipmentComponent::ServerAddItemFromInventory_Implementation(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex)
{
	UIFItemBase* Item = Source->GetItem(SourceIndex);
	if (!Item)
		return;

	EquipmentItems[EquipmentIndex].Item = DuplicateObject<UIFItemBase>(Item, this);
	OnServerItemAdded(EquipmentItems[EquipmentIndex].Item, EquipmentIndex);
	EquipmentItems[EquipmentIndex].Item->OnServerItemAddedEquipment(EquipmentItems[EquipmentIndex].Index);
	ClientAddItemFromInventory(Source, SourceIndex, EquipmentIndex);
	OnItemAddedEvent.Broadcast(EquipmentIndex, EquipmentIndex, EquipmentItems[EquipmentIndex].Item);
	
}
bool UIFEquipmentComponent::ServerAddItemFromInventory_Validate(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex)
{
	return true;
}
void UIFEquipmentComponent::ClientAddItemFromInventory_Implementation(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex)
{
	UIFItemBase* Item = Source->GetItem(SourceIndex);
	if (!Item)
		return;

	EquipmentItems[EquipmentIndex].Item = DuplicateObject<UIFItemBase>(Item, this);
	EquipmentItems[EquipmentIndex].Item->OnItemAddedEquipment(EquipmentItems[EquipmentIndex].Index);
	OnItemAdded(EquipmentItems[EquipmentIndex].Item, EquipmentIndex);
	OnItemAddedEvent.Broadcast(EquipmentIndex, EquipmentIndex, EquipmentItems[EquipmentIndex].Item);
	Source->RemoveItem(SourceIndex);
}


void UIFEquipmentComponent::RemoveFromEquipment(uint8 EquipmentIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerRemoveFromEquipment(EquipmentIndex);
		return;
	}
}

void UIFEquipmentComponent::ServerRemoveFromEquipment_Implementation(uint8 EquipmentIndex)
{
	EquipmentItems[EquipmentIndex].Item->OnServerItemRemovedEquipment(EquipmentItems[EquipmentIndex].Index);
	if (EquipmentItems[EquipmentIndex].Item)
	{
		EquipmentItems[EquipmentIndex].Item->MarkPendingKill();
	}
	EquipmentItems[EquipmentIndex].Item = nullptr;
	ClientRemoveFromEquipment(EquipmentIndex);
	OnServerItemRemoved(EquipmentIndex);
}
bool UIFEquipmentComponent::ServerRemoveFromEquipment_Validate(uint8 EquipmentIndex)
{
	return true;
}

void UIFEquipmentComponent::ClientRemoveFromEquipment_Implementation(uint8 EquipmentIndex)
{
	EquipmentItems[EquipmentIndex].Item->OnItemRemovedEquipment(EquipmentItems[EquipmentIndex].Index);
	if (EquipmentItems[EquipmentIndex].Item)
	{
		EquipmentItems[EquipmentIndex].Item->MarkPendingKill();
	}
	EquipmentItems[EquipmentIndex].Item = nullptr;
	OnItemRemoved(EquipmentIndex);
}