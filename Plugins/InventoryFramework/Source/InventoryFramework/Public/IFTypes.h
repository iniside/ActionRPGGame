// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FIFItemEvent, uint8, uint8, class UIFItemBase*);
DECLARE_MULTICAST_DELEGATE(FIFOnInventoryChanged);

USTRUCT(BlueprintType)
struct INVENTORYFRAMEWORK_API FIFItemData
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UIFItemBase* Item;

	UPROPERTY(BlueprintReadOnly)
		uint8 Index;
};


USTRUCT(BlueprintType)
struct FIFSlotAcceptedClasses
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UIFItemBase>> AcceptedClasses;

};

/**
 * 
 */
class INVENTORYFRAMEWORK_API IFTypes
{
public:
	IFTypes();
	~IFTypes();
};
