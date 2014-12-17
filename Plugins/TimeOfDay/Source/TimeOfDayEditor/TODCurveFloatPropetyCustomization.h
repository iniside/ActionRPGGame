#pragma once
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetTypeActions_Base.h"


/*
	Customization for curve property display.

	Instead of displaying Curve, it will display name of property and edit box with value of
	of curve in current time.
	

	If there is no key in current value and value will be changed, new key will be added to curve
	at current time.
*/
class FTODCurveFloatPropetyCustomization : public IPropertyTypeCustomization
{
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};