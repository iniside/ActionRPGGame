// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "IFItemBase.generated.h"

/*
	A Struct where the actuall item is contained (so it can be easy serialized/deserialized from json. 
	Also allows to easily embed a replicate itemsh within items.
*/
USTRUCT()
struct FIFItemBaseData
{
	GENERATED_BODY()
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYFRAMEWORK_API UIFItemBase : public UObject
{
	GENERATED_BODY()
public:
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
		Called after item has been added to inventory.
	*/
	virtual void OnItemAdded(uint8 LocalIndex) {};
	/*
		Called when item changed slots within THE SAME inventory;
	*/
	virtual void OnItemChanged(uint8 LocalIndex) {};
	/*
		Called after item has been removed from inventory;
	*/
	virtual void OnItemRemoved(uint8 LocalIndex) {};
};
