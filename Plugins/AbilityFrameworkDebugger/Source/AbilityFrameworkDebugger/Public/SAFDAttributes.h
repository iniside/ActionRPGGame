// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AFAbilityInterface.h"
#include "Attributes/GAAttributesBase.h"

struct FAttributeRow : public TSharedFromThis<FAttributeRow>
{
	FString Name;
	FAFAttributeBase* Attribute;
	TAttribute<FText> Value;

	FText GetValue() const
	{
		return FText::AsNumber(Attribute->GetCurrentValue());
	}
};

/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API SAFDAttributes : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDAttributes)
	{}
		SLATE_ARGUMENT(IAFAbilityInterface*, AbilityInterface)
	SLATE_END_ARGS()

	IAFAbilityInterface* AFInterface;
	TArray<TSharedPtr<FAttributeRow>> Attributes;
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void GatherAttributes();
	TSharedRef<ITableRow> GenerateListRow(TSharedPtr<FAttributeRow> NotifyName, const TSharedRef<STableViewBase>& OwnerTable);
};
