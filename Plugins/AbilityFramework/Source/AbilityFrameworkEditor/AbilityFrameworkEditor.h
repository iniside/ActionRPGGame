#pragma once
#include "Engine.h"
#include "AbilityFramework.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "EditorStyle.h"
#include "Effects/GAEffectCueSequence.h"
#include "EffectCueEditor/GAEffectCueEditor.h"

#include "UnrealEd.h"
#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "IAbilityFrameworkEditor.h"
class FEffectCueSequenceEditorTabBinding
	: public TSharedFromThis<FEffectCueSequenceEditorTabBinding>
{
public:

	FEffectCueSequenceEditorTabBinding()
	{

	}


	~FEffectCueSequenceEditorTabBinding()
	{
		/*FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
		if (BlueprintEditorModule)
		{
		BlueprintEditorModule->OnRegisterTabsForEditor().Remove(BlueprintEditorTabSpawnerHandle);
		BlueprintEditorModule->OnRegisterLayoutExtensions().Remove(BlueprintEditorLayoutExtensionHandle);
		}*/
	}

private:

	/** Delegate binding handle for FBlueprintEditorModule::OnRegisterTabsForEditor */
	FDelegateHandle BlueprintEditorTabSpawnerHandle, BlueprintEditorLayoutExtensionHandle;

	FDelegateHandle LevelEditorTabSpawnerHandle, LevelEditorLayoutExtensionHandle;
};
//add static list of registered custom derived effcts. Will be used to specify, from which classes
//new effect blueprints can be created, and which classes are allowed to pick (if not specified 
//in metaData.
class FAbilityFrameworkEditor : public IAbilityFrameworkEditor
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
	TSharedPtr<FEffectCueSequenceEditorTabBinding> BlueprintEditorTabBinding;
	FDelegateHandle OnInitializeSequenceHandle;

	static TSet<UClass*> EffectClasses;

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
	static void OnInitializeSequence(UGAEffectCueSequence* Sequence);
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};