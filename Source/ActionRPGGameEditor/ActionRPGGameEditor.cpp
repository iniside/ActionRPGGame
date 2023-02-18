// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGameEditor.h"
#include "Modules/ModuleManager.h"
#include "Misc/CommandLine.h"
#include "IAbilityFrameworkEditor.h"

void FActionRPGGameEditorModule::StartupModule()
{
	//FModuleManager::Get().LoadModule("Kismet");
	FModuleManager::LoadModuleChecked<IAbilityFrameworkEditor>(TEXT("AbilityFrameworkEditor"));
};
IMPLEMENT_PRIMARY_GAME_MODULE(FActionRPGGameEditorModule, ActionRPGGameEditor, "ActionRPGGameEditor");
 