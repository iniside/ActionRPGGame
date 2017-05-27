// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "IDetailCustomization.h"
#include "PropertyHandle.h"

class FTimeOfDayActorDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

private:
	IDetailLayoutBuilder* MyDetailLayout;
	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};