// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARUMGWidgetBase.h"

#include "Components/WrapBox.h"

#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

#include "UI/ARUIComponent.h"

#include "ARInventoryScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARInventoryScreenWidget : public UARUMGWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* RightWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* LeftWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* SideWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* BottomBackWeaponWidget;

	/*
		Contains list of items compatibile with selected slot.
	*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* SelectedItemsContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SelectedWeapon;
protected:
	TArray<UARItemWeaponWidget*> WeaponSlots;
public:
	virtual void NativeConstruct() override;

	void SetWeaponName(const FString& Name);

	template<typename ItemType, typename WidgetType>
	void UpdateItemList(const TArray<uint8>& LocalIdxs, TSubclassOf<WidgetType> WidgetClass)
	{
		SelectedItemsContainer->ClearChildren();

		for (uint8 Idx : LocalIdxs)
		{
			ItemType* Item = Inventory->GetItem<ItemType>(Idx);

			if (Item)
			{
				const FIFItemData& Slot = Inventory->GetSlot(Idx);
				WidgetType* ItemWidget = CreateWidget<WidgetType>(PC.Get(), WidgetClass);
				ItemWidget->Inventory = Inventory;
				ItemWidget->OnSlotCreated(Slot.GetNetIndex(), Slot.GetLocalIndex());
				ItemWidget->OnItemChanged(Slot.GetNetIndex(), Slot.GetLocalIndex());

				SelectedItemsContainer->AddChild(ItemWidget);
			}
		}
	}

	template<typename ItemType, typename WidgetType>
	void AddItems(const TArray<ItemType*>& Items, TSubclassOf<WidgetType> WidgetClass, class AARPlayerController* PC, class UARItemView* ForSlot)
	{
		SelectedItemsContainer->ClearChildren();

		uint8 Idx = 0;
		for (ItemType* Item : Items)
		{
			if (Item)
			{
				const FIFItemData& Slot = PC->MainInventory->GetSlot(Idx);
				WidgetType* ItemWidget = CreateWidget<WidgetType>(PC, WidgetClass);
				
				ItemWidget->SetTarget(ForSlot);
				ItemWidget->OnSlotCreated(Slot.GetNetIndex(), Slot.GetLocalIndex(), Item);
				ItemWidget->OnItemChanged(Slot.GetNetIndex(), Slot.GetLocalIndex(), Item);

				SelectedItemsContainer->AddChild(ItemWidget);
			}
			Idx++;
		}
	}

	void OnWeaponAdded(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);
	void OnWeaponUpdated(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);
	void OnWeaponRemoved(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item);
};
