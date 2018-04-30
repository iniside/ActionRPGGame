// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "GACurveTableDetailCustomization.h"

TSharedRef<IPropertyTypeCustomization> FGACurveTableDetailCustomization::MakeInstance()
{
	return MakeShareable(new FGACurveTableDetailCustomization);
}

FGACurveTableDetailCustomization::~FGACurveTableDetailCustomization()
{

}

void FGACurveTableDetailCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}
void FGACurveTableDetailCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}
