// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GAGlobalTypesEditor.h"

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FGACurveTableDetailCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	/**
	* Destructor
	*/
	virtual ~FGACurveTableDetailCustomization();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;



};