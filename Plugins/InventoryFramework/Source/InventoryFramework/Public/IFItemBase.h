// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "IFItemBase.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYFRAMEWORK_API UIFItemBase : public UObject
{
	GENERATED_BODY()
public:
	
	//Possibly just one index, if we are going to bypass unreal replication
	//and just send items as json from server and then decode them and insert in map.
	UPROPERTY()
		uint8 LocalIndex;
	UPROPERTY()
		uint8 NetIndex;

	bool IsNameStableForNetworking() const override
	{
		return false;
	}

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
	bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;

	/*
		Called just fater NewObject<> On server or in standalone.
	*/
	virtual void OnServerItemLoaded() {};

	/*
		Called after item has been added to inventory.
	*/
	virtual void OnItemAdded(uint8 InIndex) {};
	/*
		Called when item changed slots within THE SAME inventory;
	*/
	virtual void OnItemChanged(uint8 InIndex) {};
	/*
		Called after item has been removed from inventory;
	*/
	virtual void OnItemRemoved(uint8 InIndex) {};

	/*
		Called after item has been added to inventory.
		Called on server or in standalone game.
	*/
	virtual void OnServerItemAdded(uint8 InIndex) {};
	/*
		Called when item changed slots within THE SAME inventory.
		Called on server or in standalone game.
	*/
	virtual void OnServerItemChanged(uint8 InIndex) {};
	/*
		Called after item has been removed from inventory.
		Called on server or in standalone game.
	*/
	virtual void OnServerItemRemoved(uint8 InIndex) {};



	/*
		Called after item has been added to Equipment.
	*/
	virtual void OnItemAddedEquipment(uint8 InIndex) {};
	/*
		Called when item changed slots within THE SAME Equipment;
	*/
	virtual void OnItemChangedEquipment(uint8 InIndex) {};
	/*
		Called after item has been removed from Equipment;
	*/
	virtual void OnItemRemovedEquipment(uint8 InIndex) {};

	/*
		Called after item has been added to Equipment.
		Called on server or in standalone game.
	*/
	virtual void OnServerItemAddedEquipment(uint8 InIndex) {};
	/*
		Called when item changed slots within THE SAME Equipment;
		Called on server or in standalone game.
	*/
	virtual void OnServerItemChangedEquipment(uint8 InIndex) {};
	/*
		Called after item has been removed from Equipment;
		Called on server or in standalone game.
	*/
	virtual void OnServerItemRemovedEquipment(uint8 InIndex) {};

	virtual void PreItemSerializeToJson() {};
	virtual void PreItemLoad() {};

	virtual void ClientPostItemDeserializeFromJson() {};
	static UIFItemBase* LoadFromJSON() { return nullptr; }

};
