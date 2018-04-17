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
public:
	virtual void NativeConstruct() override;

	inline void SetTarget(TWeakObjectPtr<class UARItemView> ForSlot) { Target = ForSlot; }
	inline TWeakObjectPtr<class UARItemView> GetTarget() { return Target; }
	
};
