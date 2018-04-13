// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IFItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYFRAMEWORKUI_API UIFItemWidget : public UUserWidget
{
	GENERATED_BODY()
		friend class UIFItemContainerWidget;
protected:
	/*
		Those indexes correponds to the client copy of items;
	*/
	UPROPERTY()
		uint8 NetIndex;
	UPROPERTY()
		uint8 LocalIndex;
	
	void OnSlotCreated(uint8 InNetINdex, uint8 InLocaIndex);

	void OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex);
};
