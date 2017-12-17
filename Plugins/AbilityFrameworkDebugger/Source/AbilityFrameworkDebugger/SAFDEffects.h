// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"

struct FAFDEffectRow
{
	FString EffectClassName;
	FGAEffectHandle Handle;

	const bool operator==(const FAFDEffectRow& Other) const
	{
		return Handle == Other.Handle;
	}

	const bool operator==(const FGAEffectHandle& InHandle) const
	{
		return Handle == InHandle;
	}
	const bool operator==(FGAEffectHandle InHandle) const
	{
		return Handle == InHandle;
	}
	const bool operator==(TSharedPtr<FAFDEffectRow> InHandle) const
	{
		return Handle == InHandle->Handle;
	}
};


/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API SAFDEffects : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDEffects)
	{}
		SLATE_ARGUMENT(IAFAbilityInterface*, AbilityInterface)
	SLATE_END_ARGS()
protected:
	IAFAbilityInterface* AFInterface;
	TWeakObjectPtr<UAFAbilityComponent> AbilityComponent;
	TSharedPtr<SListView<TSharedPtr<FAFDEffectRow>>> ListView;
	FDelegateHandle OnEffectAppliedHandle;
	FDelegateHandle OnEffectRemovedHandle;
	FDelegateHandle OnEffectExpiredHandle;

	TArray<TSharedPtr<FAFDEffectRow>> Effects;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	~SAFDEffects();
protected:
	TSharedRef<ITableRow> GenerateListRow(TSharedPtr<FAFDEffectRow> EffectRow, const TSharedRef<STableViewBase>& OwnerTable);
	void InitializeEffects();
	void OnEffectApplied(FGAEffectHandle InHandle);
	void OnEffectRemoved(FGAEffectHandle InHandle);
};
