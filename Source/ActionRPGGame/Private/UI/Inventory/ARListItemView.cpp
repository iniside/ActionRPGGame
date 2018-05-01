// Fill out your copyright notice in the Description page of Project Settings.

#include "ARListItemView.h"

#include "UI/ARHUD.h"
#include "ARPlayerController.h"


void UARListItemView::NativeConstruct()
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