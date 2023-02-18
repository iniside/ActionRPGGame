// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "AbilityFrameworkEditor.h"

#include "KismetEditorUtilities.h"

#include "STextComboBox.h"
#include "STreeView.h"
#include "GAGlobalTypesEditor.h"

#include "Attributes/GAAttributesBase.h"
#include "SGAAttributeWidget.h"

void SGAAttributeWidget::Construct(const FArguments& InArgs)
{
	AttributesList.Empty();
	AttributesNodes.Empty();
	OnAttributeSelected = InArgs._OnAttributeSelectedIn;

	/*
		Intended look:
		ClassName
		--CategoryName
		----AttributeName

		Or:
		ClassName
		--CategoryName
		----AttributeType
		------AttributeName

		+ search. We really need search!
	*/
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (Class->IsChildOf(UGAAttributesBase::StaticClass())
			&& !FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		{
			FString className = Class->GetName();
			if (!className.Contains(TEXT("REINST_")))
			{
				TSharedPtr<FGAAttributeNode> classRootNode = MakeShareable(new FGAAttributeNode());
				classRootNode->Attribute = className;
				//first let's find root of tree, which is class name
				classRootNode->NodeName = className;
				FString Category;
				//now let's categories as childs
				for (TFieldIterator<UProperty> PropertyIt(Class, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
				{
					UProperty* Prop = *PropertyIt;
					Category = Prop->GetMetaData("Category");

					//check if we already have this category added..
					bool bFoundExistingCategory = false;
					if (!Category.IsEmpty())
					{
						for (TSharedPtr<FGAAttributeNode> childNode : classRootNode->ChildNodes)
						{
							if (childNode->NodeName == Category)
							{
								bFoundExistingCategory = true;
								TSharedPtr<FGAAttributeNode> attrNode = MakeShareable(new FGAAttributeNode());
								attrNode->NodeName = Prop->GetName();
								attrNode->ParentNode = childNode;
								childNode->ChildNodes.Add(attrNode);
							}
						}
						if (!bFoundExistingCategory)
						{
							TSharedPtr<FGAAttributeNode> categoryNode = MakeShareable(new FGAAttributeNode());
							categoryNode->NodeName = Category;
							categoryNode->ParentNode = classRootNode;
							classRootNode->ChildNodes.Add(categoryNode);

							TSharedPtr<FGAAttributeNode> attrNode = MakeShareable(new FGAAttributeNode());
							attrNode->NodeName = Prop->GetName();
							attrNode->ParentNode = categoryNode;
							categoryNode->ChildNodes.Add(attrNode);
						}
					}
				}
				AttributesNodes.Add(classRootNode);
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
	if (SelectedItem.IsValid())
	{
		if (OnAttributeSelected.IsBound())
		{
			OnAttributeSelected.Execute(SelectedItem->NodeName);
		}
	}
}

TSharedRef<ITableRow> SGAAttributeWidget::OnGenerateRow(TSharedPtr<FGAAttributeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow< TSharedPtr<FGAAttributeNode> >, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InItem->NodeName))
		];
}

void SGAAttributeWidget::OnGetChildren(TSharedPtr<FGAAttributeNode> InItem, TArray< TSharedPtr<FGAAttributeNode> >& OutChildren)
{
	TArray<TSharedPtr<FGAAttributeNode>> Children;

	TArray<TSharedPtr<FGAAttributeNode>> Children2;

	for (const FString& attribute : InItem->AttributeNames)
	{
		TSharedPtr<FGAAttributeNode> attrNode = MakeShareable(new FGAAttributeNode());
		attrNode->Attribute = attribute;
		Children.Add(attrNode);
	}

	for (TSharedPtr<FGAAttributeNode> childs : InItem->ChildNodes)
	{
		Children2.Add(childs);
	}

	OutChildren += Children2;
}


void SGAAttributeWidget::OnExpansionChanged(TSharedPtr<FGAAttributeNode> InItem, bool bIsExpanded)
{

}


