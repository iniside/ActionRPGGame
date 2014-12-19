// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../Public/GameAttributesEditor.h"
#include "GAAttributePin.h"
#include "GAGlobalTypes.h"
#include "GAAttributePanelGraphPinFactory.h"


#include "GameAttributesEditorPrivatePCH.h"


class FGameAttributesEditor : public IGameAttributesEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		TSharedPtr<FGAAttributePanelGraphPinFactory> GAAttributePanelGraphPinFactory = MakeShareable(new FGAAttributePanelGraphPinFactory());

		FEdGraphUtilities::RegisterVisualPinFactory(GAAttributePanelGraphPinFactory);
	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_MODULE(FGameAttributesEditor, GameAttributesEditor)


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



