// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemView.h"

#include "UI/ARHUD.h"
#include "ARPlayerController.h"


void UARItemView::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer())
	{
		if (AARHUD* HUD = Cast<AARHUD>(GetOwningPlayer()->GetHUD()))
		{
			InventoryComponent = HUD->GetUIInventory();
		}
	}
}
