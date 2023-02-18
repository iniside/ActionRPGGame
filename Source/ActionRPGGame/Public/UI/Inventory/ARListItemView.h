// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemWidget.h"
#include "ARListItemView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARListItemView : public UIFItemWidget
{
	GENERATED_BODY()
	
protected:
	TWeakObjectPtr<class UARItemView> Target;
	TWeakObjectPtr<class UARUIInventoryComponent> InventoryComponent;

	TWeakObjectPtr<class UARItemBase> InvItem;

	UPROPERTY(Transient)
		class UARItemTooltipView* ItemTooltip;
public:
	virtual void NativeConstruct() override;

	inline void SetTarget(TWeakObjectPtr<class UARItemView> ForSlot) { Target = ForSlot; }
	inline TWeakObjectPtr<class UARItemView> GetTarget() { return Target; }

	virtual void OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item) override;

	virtual void OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item) override;

	virtual void OnItemRemoved(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
