// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "SGAAttributeWidget.h"

#include "GAAttributeDetailCustomization.h"

TSharedRef<IPropertyTypeCustomization> FGAAttributeDetailCustomization::MakeInstance()
{
	return MakeShareable(new FGAAttributeDetailCustomization);
}

FGAAttributeDetailCustomization::~FGAAttributeDetailCustomization()
{

}

void FGAAttributeDetailCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	SocketNameHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGAAttribute, AttributeName));
	check(SocketNameHandle.IsValid());

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		.MaxDesiredWidth(512)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &FGAAttributeDetailCustomization::GetSocketTree)
				.ContentPadding(FMargin(0.4))
				.ButtonContent()
				[
					SNew(STextBlock)
					.Margin(FMargin(0.6))
					.Text(this, &FGAAttributeDetailCustomization::GetAttributeName)
				]
			]
		];
}
void FGAAttributeDetailCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}

FText FGAAttributeDetailCustomization::GetAttributeName() const
{
	FName attributeName;
	if (SocketNameHandle.IsValid())
	{
		SocketNameHandle->GetValue(attributeName);
		return FText::FromName(attributeName);
	}
	return FText::FromName(attributeName);
}

TSharedRef<SWidget> FGAAttributeDetailCustomization::GetSocketTree()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.MaxHeight(400)
		[
			SNew(SGAAttributeWidget)
			.OnAttributeSelectedIn(this, &FGAAttributeDetailCustomization::SetSocketName)
		];
}

void FGAAttributeDetailCustomization::SetSocketName(FString AttributeNameIn)
{
	FName name;
	FName nameToSet = *AttributeNameIn;
	SocketNameHandle->GetValue(name);
	if (name != nameToSet)
	{
		SocketNameHandle->SetValue(nameToSet);
	}
}