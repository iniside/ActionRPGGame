// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameTraceEditor.h"
#include "PropertyEditorModule.h"
//#include "../SocketName/GTSocketNameDetailCustomization.h"


#include "IGameTraceEditor.h"


class FGameTraceEditor : public IGameTraceEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		//PropertyModule.RegisterCustomPropertyTypeLayout("SocketName", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGTSocketNameDetailCustomization::MakeInstance));
	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameTraceEditor, GameTraceEditor)


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



