// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FAbilityFrameworkDebuggerModule : public IModuleInterface
{
private:
	IConsoleObject* CommandOpenAbilityDebugger;
	static void OpenAbilityDebugger();
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITORONLY_DATA
	void HandlePIEEnd(bool InVal);
#endif
};