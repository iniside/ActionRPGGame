// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"

#include "KismetEditorUtilities.h"

#include "STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "Attributes/GAAttributesBase.h"
#include "SGAAttributeWidget.h"
#include "GAAttributePin.h"

void SGAAttributePin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	AttributesList.Empty();

	SelectedAttribute = InGraphPinObj->GetDefaultAsString();
	SelectedAttribute.RemoveFromStart("(AttributeName=\"");
	SelectedAttribute.RemoveFromEnd("\")");

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (Class->IsChildOf(UGAAttributesBase::StaticClass())
			&& !FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		{
			for (TFieldIterator<UProperty> PropertyIt(Class, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
			{
				UProperty* Prop = *PropertyIt;
				//I need array within array, one for list of attributes, and one for class names.
				TSharedPtr<FString> attribute = MakeShareable(new FString(Prop->GetName()));
				AttributesList.Add(attribute);
			}
		}
	}
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}
TSharedRef<SWidget>	SGAAttributePin::GetDefaultValueWidget()
{
	//AttributesList.Empty();
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(AttributeComboButton, SComboButton)
			.OnGetMenuContent(this, &SGAAttributePin::GetAttributesMenu)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &SGAAttributePin::GetSelectedAttributeText)
			]
		];
	//return	SNew(STextComboBox)
	//	.OptionsSource(&AttributesList)
	//	.OnSelectionChanged(this, &SGAAttributePin::OnAttributeSelected);
	//return AttributeComboButton.ToSharedRef();
		
}

TSharedRef<SWidget> SGAAttributePin::GetAttributesMenu()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.MaxHeight(400)
		[
			SNew(SGAAttributeWidget)
			.OnAttributeSelectedIn(this, &SGAAttributePin::OnAttributeSelected)
		];
}

FText SGAAttributePin::GetSelectedAttributeText() const
{
	return FText::FromString(SelectedAttribute);
}
void SGAAttributePin::OnAttributeSelected(FString ItemSelected)
{
	SelectedAttribute = ItemSelected;
	//FString CurrentValue = GraphPinObj->GetDefaultAsString();
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	FString attribute = ItemSelected;
	if (CurrentDefaultValue.IsEmpty())
	{
		CurrentDefaultValue = FString(TEXT("()"));
	}
	FString AttributeString = TEXT("(");
	if (!attribute.IsEmpty())
	{
		AttributeString += TEXT("AttributeName=\"");
		AttributeString += attribute;
		AttributeString += TEXT("\"");
	}
	AttributeString += TEXT(")");

	if (!CurrentDefaultValue.Equals(AttributeString))
	{
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, AttributeString);
	}

	//if ()
}