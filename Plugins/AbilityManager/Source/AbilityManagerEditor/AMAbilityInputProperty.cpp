// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "AMAbilityInputProperty.h"
#include "AbilityManagerEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "Editor/PropertyEditor/Private/IDetailsViewPrivate.h"
#include "Editor/PropertyEditor/Private/SDetailsViewBase.h"
#include "Editor/PropertyEditor/Private/SDetailsView.h"
//D:\Unreal\UnrealEngine-Master\Engine\Source\Editor\PropertyEditor\Private\SDetailsView.h
#include "STextCombobox.h"
#include "STreeView.h"
#include "SButton.h"
#include "STextBlock.h"

#include "EditorClassUtils.h"


TSharedRef<IPropertyTypeCustomization> FAMAbilityInputProperty::MakeInstance()
{
	return MakeShareable(new FAMAbilityInputProperty);
}

FAMAbilityInputProperty::~FAMAbilityInputProperty()
{

}

void FAMAbilityInputProperty::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}
void FAMAbilityInputProperty::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}
