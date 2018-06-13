// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "Modules/ModuleManager.h"

#if ENABLE_GRPC
#include "IGRPC.h"
#include "grpc++/grpc++.h"
#endif

#if WITH_EDITOR
#include "IAbilityFrameworkEditor.h"
#endif
void FActionRPGGameModule::StartupModule()
{
#if ENABLE_GRPC
	FModuleManager::Get().LoadModule(TEXT("GRPC"));
	grpc::string out = grpc::Version();
	FString ver = FString(ANSI_TO_TCHAR(out.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("ActionRPGGame GRPC Version: %s "), *ver);
#endif
	
#if WITH_EDITOR
	FModuleManager::Get().LoadModule(TEXT("AbilityFrameworkEditor"));
	//FModuleManager::LoadModuleChecked<IAbilityFrameworkEditor>(TEXT("AbilityFrameworkEditor"));
#endif //WITH_EDITOR
};
IMPLEMENT_PRIMARY_GAME_MODULE(FActionRPGGameModule, ActionRPGGame, "ActionRPGGame");
 