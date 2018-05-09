// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARUMGWidgetBase.h"

#include "Components/WrapBox.h"

#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

#include "UI/ARUIComponent.h"
#include "ARPlayerController.h"
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

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		UButton* ModifySelectedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemMagazineView* MagazineUpgrade;


	/*
		Contains list of items compatibile with selected slot.
	*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* SelectedItemsContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* WeaponModificationContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SelectedWeapon;

	TWeakObjectPtr<class UARUIInventoryComponent> Inventory;
public:
	virtual void NativeConstruct() override;

	void SetWeaponName(const FString& Name);

	template<typename ItemType, typename WidgetType>
	void UpdateItemList(const TArray<uint8>& LocalIdxs, TSubclassOf<WidgetType> WidgetClass, class AARPlayerController* PC, class UARItemView* ForSlot)
	{
		SelectedItemsContainer->ClearChildren();

		for (uint8 Idx : LocalIdxs)
		{
			ItemType* Item = PC->MainInventory->GetItem<ItemType>(Idx);

			if (Item)
			{
				const FIFItemData& Slot = PC->MainInventory->GetSlot(Idx);
				WidgetType* ItemWidget = CreateWidget<WidgetType>(PC, WidgetClass);
				ItemWidget->SetTarget(ForSlot);
				ItemWidget->OnSlotCreated(Slot.Index, Slot.Index, Item);
				ItemWidget->OnItemChanged(Slot.Index, Slot.Index, Item);

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
				ItemWidget->OnSlotCreated(Slot.Index, Slot.Index, Item);
				ItemWidget->OnItemChanged(Slot.Index, Slot.Index, Item);

				SelectedItemsContainer->AddChild(ItemWidget);
			}
			Idx++;
		}
	}

	template<typename ItemType, typename WidgetType>
	void AddWeaponMods(const TArray<uint8>& Items, TSubclassOf<WidgetType> WidgetClass, class AARPlayerController* PC, class UARItemView* ForSlot)
	{
		WeaponModificationContainer->ClearChildren();

		for (uint8 Idx : Items)
		{
			ItemType* Item = PC->MainInventory->GetItem<ItemType>(Idx);

			if (Item)
			{
				const FIFItemData& Slot = PC->MainInventory->GetSlot(Idx);
				WidgetType* ItemWidget = CreateWidget<WidgetType>(PC, WidgetClass);
				
				ItemWidget->OnSlotCreated(Slot.Index, Slot.Index, Item);
				ItemWidget->OnItemChanged(Slot.Index, Slot.Index, Item);

				WeaponModificationContainer->AddChild(ItemWidget);
			}
		}
	}

protected:
	UFUNCTION()
		void OnModifyWeaponClicked();
};
