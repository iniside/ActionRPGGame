// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameAttributes.h"
#include "GameAttributesPrivatePCH.h"

DEFINE_LOG_CATEGORY(GameAttributesGeneral);
DEFINE_LOG_CATEGORY(GameAttributes);
DEFINE_LOG_CATEGORY(GameAttributesEffects);

class FGameAttributes : public IGameAttributes
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FGameAttributes, GameAttributes )



void FGameAttributes::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FGameAttributes::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



