// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"

#include "SARFTItem.h"
#include "WidgetLayoutLibrary.h"
#include "SARFTContainer.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARFTContainer::Construct(const FArguments& InArgs)
{
	OwningPC = InArgs._PC;

	for (int32 idx = 0; idx < InArgs._PoolSize; idx++)
	{
		TSharedPtr<SARFTItem> NewItem = SNew(SARFTItem).OwningComp(InArgs._OwningComp);//.Config(InArgs._Config);
		NewItem->SetVisibility(EVisibility::Collapsed);
		NewItem->Container = this;
		ItemsPool.Enqueue(NewItem);
		GEngine->GameViewport->AddViewportWidgetContent(NewItem.ToSharedRef());
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARFTContainer::EmitText(FVector WorldPos, const FString& InText)
{
	FVector OutPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPositionWithDistance(OwningPC.Get(), WorldPos, OutPosition);
	TSharedPtr<SARFTItem> Item;
	ItemsPool.Dequeue(Item);
	UsedItemsPool.Enqueue(Item);
	Item->Text = InText;
	Item->Position = FVector2D(OutPosition.X, OutPosition.Y); //OutPosition;
	Item->Refresh();
}

void SARFTContainer::Reset()
{
	while (!ItemsPool.IsEmpty())
	{
		TSharedPtr<SARFTItem> Item;
		ItemsPool.Dequeue(Item);
		GEngine->GameViewport->RemoveViewportWidgetContent(Item.ToSharedRef());
		Item->Reset();
		Item.Reset();
	}

	while (!UsedItemsPool.IsEmpty())
	{
		TSharedPtr<SARFTItem> Item;
		UsedItemsPool.Dequeue(Item);
		GEngine->GameViewport->RemoveViewportWidgetContent(Item.ToSharedRef());
		Item->Reset();
		Item.Reset();
	}
}