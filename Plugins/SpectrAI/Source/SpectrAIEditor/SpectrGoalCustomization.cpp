// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SpectrGoalCustomization.h"

TSharedRef<IDetailCustomization> FSpectrGoalCustomization::MakeInstance()
{
	return MakeShareable(new FSpectrGoalCustomization());
}
void FSpectrGoalCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{

}