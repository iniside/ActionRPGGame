// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IFTypes.h"
#include "IFItemBase.h"
#include "IFEquipmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYFRAMEWORK_API UIFEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
		TArray<FIFItemData> EquipmentItems;


	UPROPERTY(EditAnywhere, Category = "Inventory")
		uint8 MaxSlots;

	/*
	Currently available slots (must be smaller or equal to max slots);
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		uint8 AvailableSlots;

	FIFItemEvent OnItemAddedEvent;
	FIFItemEvent OnItemUpdatedEvent;
	FIFItemEvent OnItemRemovedEvent;

public:	
	// Sets default values for this component's properties
	UIFEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	inline UIFItemBase* GetItem(uint8 InLocalIndex)
	{
		return EquipmentItems[InLocalIndex].Item;
		//return Inventory.Items[InLocalIndex].Item;
	}
	template<typename T>
	T* GetItem(uint8 InLocalIndex)
	{
		if (EquipmentItems.IsValidIndex(InLocalIndex))
		{
			return Cast<T>(EquipmentItems[InLocalIndex].Item);
		}
		else
		{
			return nullptr;
		}
		//return Cast<T>(Inventory.Items[InLocalIndex].Item);
	}
		
	void AddItemFromInventory(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromInventory(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);
	void ServerAddItemFromInventory_Implementation(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);
	bool ServerAddItemFromInventory_Validate(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);
	/*
	Confirm that change can be made and do the same change on client.
	We do not predict inventory modifications. Clients MUST wait for server to make changes and send confirmation back.
	*/
	UFUNCTION(Client, Reliable)
		void ClientAddItemFromInventory(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);
	void ClientAddItemFromInventory_Implementation(class UIFInventoryComponent* Source, uint8 SourceIndex, uint8 EquipmentIndex);

	void RemoveFromEquipment(uint8 EquipmentIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRemoveFromEquipment(uint8 EquipmentIndex);
	void ServerRemoveFromEquipment_Implementation(uint8 EquipmentIndex);
	bool ServerRemoveFromEquipment_Validate(uint8 EquipmentIndex);
	UFUNCTION(Client, Reliable)
		void ClientRemoveFromEquipment(uint8 EquipmentIndex);
	void ClientRemoveFromEquipment_Implementation(uint8 EquipmentIndex);

	/*
		Called on client, before request to server is send to add item to Equipment component.
		Not called in standalone.
	*/
	virtual void OnClientPreItemAdded(UIFItemBase* Item, uint8 Index) {};
	virtual void OnItemAdded(UIFItemBase* Item, uint8 Index) {};
	virtual void OnItemChanged(UIFItemBase* Item, uint8 Index) {};
	virtual void OnItemRemoved(uint8 Index) {};

	//called only on server.
	virtual void OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnServerItemChanged(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnServerItemRemoved(uint8 LocalIndex) {};

	inline FIFItemEvent& GetOnItemAdded() { return OnItemAddedEvent; }
	inline FIFItemEvent& GetOnItemUpdated() { return OnItemUpdatedEvent; }
	inline FIFItemEvent& GetOnItemRemoved() { return OnItemRemovedEvent; }
};
