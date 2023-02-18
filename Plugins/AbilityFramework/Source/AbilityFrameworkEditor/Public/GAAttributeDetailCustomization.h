// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FGAAttributeDetailCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	/**
	* Destructor
	*/
	virtual ~FGAAttributeDetailCustomization();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


protected:
	/** Cached Property Handle */
	TSharedPtr<IPropertyHandle> SocketNameHandle;

	/** Used with Combobox, show up socket tree widget. */
	TSharedRef<SWidget> GetSocketTree();

	FText GetAttributeName() const;

	void SetSocketName(FString AttributeNameIn);
};