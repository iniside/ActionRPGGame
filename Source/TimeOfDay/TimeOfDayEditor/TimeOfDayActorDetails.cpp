// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"
#include "TimeOfDayActor.h"
#include "TimeOfDayActorDetails.h"
TSharedRef<IDetailCustomization> FTimeOfDayActorDetails::MakeInstance()
{
	return MakeShareable(new FTimeOfDayActorDetails);
}

void FTimeOfDayActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyDetailLayout = &DetailLayout;
	DetailLayout.HideCategory(TEXT("Mobile"));
	DetailLayout.HideCategory(TEXT("Actor"));
	DetailLayout.HideCategory(TEXT("Input"));
	DetailLayout.HideCategory(TEXT("DistanceFieldAmbientOcclusion"));
	DetailLayout.HideCategory(TEXT("LightFunction"));
	DetailLayout.HideCategory(TEXT("Performance"));
	DetailLayout.HideCategory(TEXT("Lightmass"));
	DetailLayout.HideCategory(TEXT("Replication"));
	DetailLayout.HideCategory(TEXT("DistanceFieldShadows"));
	DetailLayout.HideCategory(TEXT("LightShafts"));
	DetailLayout.HideCategory(TEXT("Cooking"));
	DetailLayout.HideCategory(TEXT("CascadedShadowMaps"));
	DetailLayout.HideCategory(TEXT("Variable"));
	DetailLayout.HideCategory(TEXT("Activation"));
	DetailLayout.HideCategory(TEXT("Rendering"));
	DetailLayout.HideCategory(TEXT("ComponentReplication"));
	DetailLayout.HideCategory(TEXT("Tags"));
	DetailLayout.HideCategory(TEXT("Lighting"));
	DetailLayout.HideCategory(TEXT("Collision"));
	DetailLayout.HideCategory(TEXT("Physics"));
	DetailLayout.HideCategory(TEXT("ComponentTick"));
	DetailLayout.HideCategory(TEXT("Actor Tick"));
}
