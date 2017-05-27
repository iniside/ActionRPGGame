// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "AbilityFrameworkEditor.h"
#include "GAAttributePin.h"
#include "Attributes/GAAttributeGlobals.h"
#include "Effects/GAEffectCue.h"
#include "GAAttributePanelGraphPinFactory.h"
#include "GAAttributeDetailCustomization.h"

#include "GAEffectDetails.h"
#include "GAEffectSpecStructCustomization.h"
#include "GAEffectPropertyStructCustomization.h"

#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "EffectEditor/AssetTypeActions_GAEffectBlueprint.h"
#include "AbilityEditor/AssetTypeActions_GAAbilityBlueprint.h"
#include "EffectCueEditor/AssetTypeActions_GAEffectCueBlueprint.h"
#include "Effects/GAEffectCueSequence.h"
#include "EffectCueEditor/GAEffectCueEditor.h"
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
class FAbilityFrameworkEditor : public IAbilityFrameworkEditor
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
	TSharedPtr<FEffectCueSequenceEditorTabBinding> BlueprintEditorTabBinding;
	FDelegateHandle OnInitializeSequenceHandle;
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	}
	static void OnInitializeSequence(UGAEffectCueSequence* Sequence)
	{
		auto* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
		AGAEffectCue* Cue = Cast<AGAEffectCue>(Sequence->GetOuter());
		if (Cue)
		{
			Sequence->GetMovieScene()->SetPlaybackRange(Cue->StartTime, Cue->EndTime);
		}
		else
		{
			Sequence->GetMovieScene()->SetPlaybackRange(ProjectSettings->DefaultStartTime, ProjectSettings->DefaultStartTime + ProjectSettings->DefaultDuration);
		}
	}
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout("GAAttribute", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGAAttributeDetailCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("GAEffectClass", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGAEffectSpecStructCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("GAEffectProperty", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGAEffectPropertyStructCustomization::MakeInstance));

		TSharedPtr<FGAAttributePanelGraphPinFactory> GAAttributePanelGraphPinFactory = MakeShareable(new FGAAttributePanelGraphPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(GAAttributePanelGraphPinFactory);

		PropertyModule.RegisterCustomClassLayout("GAGameEffectSpec", FOnGetDetailCustomizationInstance::CreateStatic(&FGAEffectDetails::MakeInstance));

		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		TSharedRef<IAssetTypeActions> GABAction = MakeShareable(new FAssetTypeActions_GAEffectBlueprint());
		RegisterAssetTypeAction(AssetTools, GABAction);
		TSharedRef<IAssetTypeActions> GAAbilityAction = MakeShareable(new FAssetTypeActions_GAAbilityBlueprint());
		RegisterAssetTypeAction(AssetTools, GAAbilityAction);
		TSharedRef<IAssetTypeActions> GAEffectCueAction = MakeShareable(new FAssetTypeActions_GAEffectCueBlueprint());
		RegisterAssetTypeAction(AssetTools, GAEffectCueAction);

		//BlueprintEditorTabBinding = MakeShared<FEffectCueSequenceEditorTabBinding>();
		OnInitializeSequenceHandle = UGAEffectCueSequence::OnInitializeSequence().AddStatic(FAbilityFrameworkEditor::OnInitializeSequence);
	}
	virtual void ShutdownModule() override
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("GAGameEffectSpec");

		PropertyModule.UnregisterCustomPropertyTypeLayout("GAAttribute");
		PropertyModule.UnregisterCustomPropertyTypeLayout("GAEffectClass");
		UGAEffectCueSequence::OnInitializeSequence().Remove(OnInitializeSequenceHandle);
		BlueprintEditorTabBinding = nullptr;
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			for (auto& AssetTypeAction : CreatedAssetTypeActions)
			{
				if (AssetTypeAction.IsValid())
				{
					AssetToolsModule.UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
				}
			}
		}
	}
};

IMPLEMENT_GAME_MODULE(FAbilityFrameworkEditor, AbilityFrameworkEditor);


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



