// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "Modules/ModuleManager.h"
#if WITH_EDITOR
#include "IAbilityFrameworkEditor.h"
#endif
void FActionRPGGameModule::StartupModule()
{
#if WITH_EDITOR
	FModuleManager::Get().LoadModule(TEXT("AbilityFrameworkEditor"));
	//FModuleManager::LoadModuleChecked<IAbilityFrameworkEditor>(TEXT("AbilityFrameworkEditor"));
#endif //WITH_EDITOR
};
IMPLEMENT_PRIMARY_GAME_MODULE(FActionRPGGameModule, ActionRPGGame, "ActionRPGGame");
 