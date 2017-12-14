// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkDebugger.h"
#include "HAL/IConsoleManager.h"
#include "AFDManager.h"

#define LOCTEXT_NAMESPACE "FAbilityFrameworkDebuggerModule"

void FAbilityFrameworkDebuggerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	CommandOpenAbilityDebugger = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAbilityDebugger"),
		TEXT("Opens ability debugger"),
		FConsoleCommandDelegate::CreateStatic(OpenAbilityDebugger),
		ECVF_Default
	);
}

void FAbilityFrameworkDebuggerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	IConsoleManager::Get().UnregisterConsoleObject(CommandOpenAbilityDebugger);
}

void FAbilityFrameworkDebuggerModule::OpenAbilityDebugger()
{
	FAFDManager::Get();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAbilityFrameworkDebuggerModule, AbilityFrameworkDebugger)