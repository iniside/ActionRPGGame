// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbilitiesComponent.h"
#include "GSAbilityTabWidget.h"
#include "GSAbilityWidget.h"
#include "GSAbilitySlotWidget.h"
#include "../../GSPlayerController.h"

#include "GSAbilityContainerWidget.h"

UGSAbilityContainerWidget::UGSAbilityContainerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsWidgetCreated = false;
}

void UGSAbilityContainerWidget::InitializeWidget()
{
	AbilityComponent->OnAbilityAddedToSet.BindUObject(this, &UGSAbilityContainerWidget::UpdateCreatetWidget);
	//AbilityComponent->OnAbilityAddedToSet.BindUObject(this, &UGSAbilityContainerWidget::UpdateCreatetWidget);
}

void UGSAbilityContainerWidget::UpdateCreatetWidget()
{
	AGSPlayerController* MyPC = AbilityComponent->PCOwner;
	if (!bIsWidgetCreated)
	{
		for (FGSAbilitiesSets& abSet : AbilityComponent->AbilitySets)
		{
			UGSAbilityTabWidget* setWid = CreateWidget<UGSAbilityTabWidget>(MyPC, AbilityTabClass);
			for (FGSAbilitySlot& slot : abSet.AbilitySlots)
			{
				UGSAbilitySlotWidget* abSlot = CreateWidget<UGSAbilitySlotWidget>(MyPC, AbilitySlotClass);
				abSlot->SlotInfo = slot;
				abSlot->OwnerComp = AbilityComponent;
				UGSAbilityWidget* abil = CreateWidget<UGSAbilityWidget>(MyPC, AbilityWidgetClass);
				abil->AbilityComponent = AbilityComponent;
				if (slot.AbilityIndex != INDEX_NONE)
				{
					abil->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					abil->AbilityIndex = slot.AbilityIndex;
					abil->UpdateWidgetData();
				}
				else
				{
					abil->SetVisibility(ESlateVisibility::Collapsed);
					abil->AbilityIndex = slot.AbilityIndex;
				}
				if (DropSlotName != NAME_None)
				{
					UWidget* superWidget = abSlot->GetWidgetFromName(DropSlotName);
					UOverlay* overlay = Cast<UOverlay>(superWidget);
					overlay->AddChild(abil);
				}

				abSlot->Ability = abil;
				setWid->AbilitySlots.Add(abSlot);
			}
			AbilitySets.Add(setWid);
		}
		bIsWidgetCreated = true;
	}
	else
	{
		for (FGSAbilitiesSets& abSet : AbilityComponent->AbilitySets)
		{
			for (FGSAbilitySlot& slot : abSet.AbilitySlots)
			{
				UGSAbilityWidget* abWid = AbilitySets[slot.SetIndex]->AbilitySlots[slot.SlotIndex]->Ability;

				if (slot.AbilityIndex != INDEX_NONE)
				{
					abWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					abWid->AbilityIndex = slot.AbilityIndex;
					abWid->UpdateWidgetData();
				}
				else
				{
					abWid->SetVisibility(ESlateVisibility::Collapsed);
					abWid->AbilityIndex = slot.AbilityIndex;
				}
			}
		}
	}
}

void UGSAbilityContainerWidget::UpdateWidget()
{
	for (FGSAbilitiesSets& abSet : AbilityComponent->AbilitySets)
	{
		for (FGSAbilitySlot& slot : abSet.AbilitySlots)
		{
			UGSAbilityWidget* abWid = AbilitySets[slot.SetIndex]->AbilitySlots[slot.SlotIndex]->Ability;

			if (slot.AbilityIndex != INDEX_NONE)
			{
				abWid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				abWid->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}
