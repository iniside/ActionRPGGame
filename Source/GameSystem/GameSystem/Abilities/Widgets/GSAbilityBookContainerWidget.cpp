// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbilitiesComponent.h"
#include "GSAbilityBookTabWidget.h"
#include "GSAbilityBookItem.h"
#include "GSAbilityBookSlotWidget.h"
#include "../../GSPlayerController.h"

#include "GSAbilityBookContainerWidget.h"

UGSAbilityBookContainerWidget::UGSAbilityBookContainerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWidgetCreated = false;
}

void UGSAbilityBookContainerWidget::InitializeWidget(const FGSAbilityBookUI& AbilityBookConfigIn,
class UGSAbilitiesComponent* CompIn)
{
	OwningComp = CompIn;
	AbilityBookConfig = AbilityBookConfigIn;

	OwningComp->OnAbilityAddedToBook.BindUObject(this, &UGSAbilityBookContainerWidget::CreateUpdateWidget);
}
void UGSAbilityBookContainerWidget::CreateUpdateWidget()
{
	AGSPlayerController* MyPC = OwningComp->PCOwner;
	if (!bWidgetCreated)
	{
		for (FGSAbilityBookTab& abSet : OwningComp->OwnedAbilities.AbilitiesTab)
		{
			UGSAbilityBookTabWidget* setWid = CreateWidget<UGSAbilityBookTabWidget>(MyPC, AbilityBookConfig.AbilityBookTabClass);
			for (FGSAbilityBookSlot& slot : abSet.Abilities)
			{
				UGSAbilityBookSlotWidget* abSlot = CreateWidget<UGSAbilityBookSlotWidget>(MyPC, AbilityBookConfig.AbilityBookSlotClass);
				UGSAbilityBookItem* abil = CreateWidget<UGSAbilityBookItem>(MyPC, AbilityBookConfig.AbilityBookClass);
				//abil->AbilityComponent = AbilityComponent;
				abSlot->SlotName = AbilityBookConfig.AbilityBookSlotName;

				abil->SetVisibility(ESlateVisibility::Collapsed);
				abil->SetIsEnabled(false);
				//abil->AbilityIndex = slot.AbilityIndex;
				if (AbilityBookConfig.AbilityBookSlotName != NAME_None)
				{
					UWidget* superWidget = abSlot->GetWidgetFromName(AbilityBookConfig.AbilityBookSlotName);
					UOverlay* overlay = Cast<UOverlay>(superWidget);
					overlay->AddChild(abil);
				}

				abSlot->AbilityItem = abil;
				setWid->TabSlots.Add(abSlot);
			}
			BookTabs.Add(setWid);
		}
		bWidgetCreated = true;
	}
	else
	{
		//ok this is not most efficient or best way to do it but, it's fairly simple, and will work well 
		//in networked environment where we simply can't be sure what data arrived, in what order, or even
		//if it is 100% accurate.
		//this works, because we assume that index mapping between components and widget matches.

		//we will simply iterate over all widgets, refreshing those, which have valid pointer to ability class.
		//we can optimized it by assuming, that if class pointer is null, then all subsequent pointers are also null.
		//and by stroing last valid index.
		//but that's later.
		int32 TabsNum = OwningComp->OwnedAbilities.AbilitiesTab.Num();
		for (int32 TabIndex = 0; TabIndex < TabsNum; TabIndex++)
		{
			FGSAbilityBookTab& TabRef = OwningComp->OwnedAbilities.AbilitiesTab[TabIndex];
			int32 SlotNum = TabRef.Abilities.Num(); //yeah we need more dots!

			for (int32 SlotIndex = 0; SlotIndex < SlotNum; SlotIndex++)
			{
				if (TabRef.Abilities[SlotIndex].AbilityClass)
				{
					BookTabs[TabIndex]->TabSlots[SlotIndex]->SlotInfo = TabRef.Abilities[SlotIndex];
					BookTabs[TabIndex]->TabSlots[SlotIndex]->AbilityItem->SetVisibility(ESlateVisibility::Visible);
					//BookTabs[TabIndex]->TabSlots[SlotIndex]->AbilityItem->SetIsEnabled(true);
					BookTabs[TabIndex]->TabSlots[SlotIndex]->AbilityItem->SlotInfo = TabRef.Abilities[SlotIndex];
				}
				else
				{
					return;
				}
			}
		}

	}
}
