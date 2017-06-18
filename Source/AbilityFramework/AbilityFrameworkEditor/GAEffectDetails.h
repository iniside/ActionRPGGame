// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IDetailCustomization.h"
#include "PropertyHandle.h"

class FGAEffectDetails : public IDetailCustomization
{

protected:
	bool bIsDuration;
	bool bIsPeriodic;
	TSharedPtr<IPropertyHandle> ApplicationTypeHandle;
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

private:
	TSharedPtr<IPropertyHandle> MyProperty;
	TSharedPtr<IPropertyHandle> DurationHandle;
	TSharedPtr<IPropertyHandle> PeriodHandle;
	TSharedPtr<IPropertyHandle> DurationCalcTypeHandle;
	TSharedPtr<IPropertyHandle> PeriodCalcTypeHandle;
	IDetailLayoutBuilder* MyDetailLayout;
	FSimpleDelegate UpdateEffectTypeyDelegate;
	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	void OnDurationPolicyChange();
};