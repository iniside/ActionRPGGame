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

#include "BlueprintEditorModule.h"
#include "BlueprintEditorTabs.h"
#include "LayoutExtender.h"
#include "LevelEditor.h"
#include "MovieSceneToolsProjectSettings.h"
#include "PropertyEditorModule.h"
#include "Styling/SlateStyle.h"
#include "WorkflowTabManager.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

#include "ISettingsModule.h"

#include "IAbilityFrameworkEditor.h"
class FEffectCueequenceEditorTabBinding
	: public TSharedFromThis<FEffectCueequenceEditorTabBinding>
{
public:

	FEffectCueequenceEditorTabBinding();

	void RegisterBlueprintEditorLayout(FLayoutExtender& Extender);

	void RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor);
	~FEffectCueequenceEditorTabBinding();

private:

	/** Delegate binding handle for FBlueprintEditorModule::OnRegisterTabsForEditor */
	FDelegateHandle BlueprintEditorTabSpawnerHandle, BlueprintEditorLayoutExtensionHandle;
};
//add static list of registered custom derived effcts. Will be used to specify, from which classes
//new effect blueprints can be created, and which classes are allowed to pick (if not specified 
//in metaData.
class FAbilityFrameworkEditor : public IAbilityFrameworkEditor
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
	TSharedPtr<FEffectCueequenceEditorTabBinding> BlueprintEditorTabBinding;
	FDelegateHandle OnInitializeSequenceHandle;
	static TSet<UClass*> EffectClasses;

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
	static void OnInitializeSequence(UGAEffectCueSequence* Sequence);
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};