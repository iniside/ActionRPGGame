// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

#include "GAEffectClassStructWidget.h"
class FGAEffectPropertyStructCustomization : public IPropertyTypeCustomization
{
protected:
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> EffectPropertyHandle;
	TSharedPtr<FGAEffectClassStructWidget> EffectClassWidget;
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	/**
	* Destructor
	*/
	virtual ~FGAEffectPropertyStructCustomization();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	FText GetClassName() const;
};