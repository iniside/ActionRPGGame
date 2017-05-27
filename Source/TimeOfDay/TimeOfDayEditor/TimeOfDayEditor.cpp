// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "TimeOfDayEditor.h"
#include "ITimeOfDayEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"

#include "BlueprintEditorModule.h"
#include "BlueprintEditorTabs.h"
#include "LayoutExtender.h"
#include "LevelEditor.h"
#include "MovieSceneToolsProjectSettings.h"
#include "PropertyEditorModule.h"
#include "Styling/SlateStyle.h"
#include "WorkflowTabManager.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

#include "TimeOfDayActorDetails.h"

class FTimeOfDayEditor : public ITimeOfDayEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FTimeOfDayEditor, TimeOfDayEditor)



void FTimeOfDayEditor::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("TimeOfDayActor", FOnGetDetailCustomizationInstance::CreateStatic(&FTimeOfDayActorDetails::MakeInstance));
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FTimeOfDayEditor::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



