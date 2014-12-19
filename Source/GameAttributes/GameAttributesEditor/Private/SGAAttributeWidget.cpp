// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameAttributesEditor.h"

#include "KismetEditorUtilities.h"

#include "STextComboBox.h"
#include "STreeView.h"
#include "GAGlobalTypesEditor.h"

#include "GAAttributesBase.h"
#include "SGAAttributeWidget.h"

void SGAAttributeWidget::Construct(const FArguments& InArgs)
{
	AttributesList.Empty();
	AttributesNodes.Empty();
	OnAttributeSelected = InArgs._OnAttributeSelectedIn;

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (Class->IsChildOf(UGAAttributesBase::StaticClass())
			&& !FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		{
			FString className = Class->GetName();
			if (!className.Contains(TEXT("REINST_")))
			{
				TSharedPtr<FGAAttributeNode> attributeNode = MakeShareable(new FGAAttributeNode());
				attributeNode->Attribute = className;
				for (TFieldIterator<UProperty> PropertyIt(Class, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
				{
					UProperty* Prop = *PropertyIt;

					//I need array within array, one for list of attributes, and one for class names.
					//	TSharedPtr<FString> attribute = MakeShareable(new FString(Prop->GetName()));
					attributeNode->AttributeNames.Add(Prop->GetName());
					//AttributesList.Add(attribute);
				}
				AttributesNodes.Add(attributeNode);
			}
		}
	}

	ChildSlot
	[
		SAssignNew(AttributeTreeWidget, STreeView<TSharedPtr<FGAAttributeNode>>)
		.OnSelectionChanged(this, &SGAAttributeWidget::OnItemSelected)
		.TreeItemsSource(&AttributesNodes)
		.OnGenerateRow(this, &SGAAttributeWidget::OnGenerateRow)
		.OnGetChildren(this, &SGAAttributeWidget::OnGetChildren)
		.OnExpansionChanged(this, &SGAAttributeWidget::OnExpansionChanged)
		.SelectionMode(ESelectionMode::Single)
	];
}
SGAAttributeWidget::~SGAAttributeWidget()
{
	AttributesNodes.Empty();
}

void SGAAttributeWidget::OnItemSelected(TSharedPtr<FGAAttributeNode> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (OnAttributeSelected.IsBound())
	{
		OnAttributeSelected.Execute(SelectedItem->Attribute);
	}
}

TSharedRef<ITableRow> SGAAttributeWidget::OnGenerateRow(TSharedPtr<FGAAttributeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow< TSharedPtr<FGAAttributeNode> >, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InItem->Attribute))
		];
}

void SGAAttributeWidget::OnGetChildren(TSharedPtr<FGAAttributeNode> InItem, TArray< TSharedPtr<FGAAttributeNode> >& OutChildren)
{
	TArray<TSharedPtr<FGAAttributeNode>> Children;
	for (const FString& attribute : InItem->AttributeNames)
	{
		TSharedPtr<FGAAttributeNode> attrNode = MakeShareable(new FGAAttributeNode());
		attrNode->Attribute = attribute;
		Children.Add(attrNode);
	}
	OutChildren = Children;
}


void SGAAttributeWidget::OnExpansionChanged(TSharedPtr<FGAAttributeNode> InItem, bool bIsExpanded)
{

}


