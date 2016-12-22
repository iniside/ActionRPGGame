// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameAbilitiesEditor.h"
#include "GAAttributePin.h"
#include "GAAttributeGlobals.h"

#include "GAAttributePanelGraphPinFactory.h"
#include "GAAttributeDetailCustomization.h"


#include "IGameAbilitiesEditor.h"


class FGameAbilitiesEditor : public IGameAbilitiesEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout("GAAttribute", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGAAttributeDetailCustomization::MakeInstance));

		TSharedPtr<FGAAttributePanelGraphPinFactory> GAAttributePanelGraphPinFactory = MakeShareable(new FGAAttributePanelGraphPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(GAAttributePanelGraphPinFactory);
	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameAbilitiesEditor, GameAbilitiesEditor);


//void FGameAttributesEditor::StartupModule()
//{
//
//}
//
//
//void FGameAttributesEditor::ShutdownModule()
//{
//
//}



