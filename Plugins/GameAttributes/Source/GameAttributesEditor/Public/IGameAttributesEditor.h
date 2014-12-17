// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "ModuleManager.h"


/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules 
 * within this plugin.
 */
class IGameAttributesEditor : public IModuleInterface
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IGameAttributesEditor& Get()
	{
		return FModuleManager::LoadModuleChecked< IGameAttributesEditor >("GameAttributesEditor");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "GameAttributesEditor" );
	}
};

