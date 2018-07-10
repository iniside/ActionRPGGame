// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "../AbilityFrameworkEditor.h"
#include "Effects/AFCueActor.h"
#include "GAEffectCueEditor.h"
#include "Effects/GAEffectCueSequence.h"
#include "EditorReimportHandler.h"
#include "MovieScene.h"
#include "Tracks/MovieScenePropertyTrack.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "ISequencerModule.h"
#include "Editor/UnrealEd/Private/Toolkits/SStandaloneAssetEditorToolkitHost.h"
#include "Toolkits/ToolkitManager.h"
#include "Toolkits/GlobalEditorCommonCommands.h"

#include "AFCueActorBlueprint.h"
#include "AFCueActorBlueprintFactory.h"
#include "GAEffectCueGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SDockTab.h"
#include "BlueprintEditorTabs.h"
#include "LayoutExtender.h"
#include "BlueprintEditorModes.h"
#include "BlueprintEditorTabs.h"

#define LOCTEXT_NAMESPACE "FGAEffectCueEditor"


//TSharedRef<SDockTab> FEffectCueSequenceEditorSummoner::SpawnTab(const FWorkflowTabSpawnInfo& Info) const
//{
//	TSharedRef<SDockTab> Tab = FWorkflowTabFactory::SpawnTab(Info);
//
//	TSharedPtr<FBlueprintEditor> BlueprintEditorPtr = StaticCastSharedPtr<FBlueprintEditor>(HostingApp.Pin());
//	BlueprintEditorPtr->GetInspector()->SetOwnerTab(Tab);
//
//	BlueprintEditorPtr->GetInspector()->GetPropertyView()->SetHostTabManager(Info.TabManager);
//
//	return Tab;
//}

/////////////////////////////////////////////////////
// FGameplayAbilitiesEditor

FGAEffectCueEditor::FGAEffectCueEditor()
{
	EditedCue = nullptr;
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorTabSpawnerHandle = BlueprintEditorModule.OnRegisterTabsForEditor().AddRaw(this, &FGAEffectCueEditor::RegisterBlueprintEditorTab);
	BlueprintEditorLayoutExtensionHandle = BlueprintEditorModule.OnRegisterLayoutExtensions().AddRaw(this, &FGAEffectCueEditor::RegisterBlueprintEditorLayout);
}

void FGAEffectCueEditor::RegisterBlueprintEditorLayout(FLayoutExtender& Extender)
{
	//Extender.ExtendLayout(FBlueprintEditorTabs::CompilerResultsID, ELayoutExtensionPosition::Before, FTabManager::FTab(FName("EmbeddedEffectCueSequenceID"), ETabState::ClosedTab));
}

void FGAEffectCueEditor::RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor)
{
	//TabFactories.RegisterFactory(MakeShared<FEffectCueSequenceEditorSummoner>(BlueprintEditor));
}
TArray<UObject*> FGAEffectCueEditor::GetAnimationEventContexts() const
{
	TArray<UObject*> EventContexts; 
	return EventContexts;
}

void FGAEffectCueEditor::OnSequenceChanged()
{
	UBlueprint* Blueprint = GetBlueprintObj();

	if (Blueprint)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	}
}
void FGAEffectCueEditor::OnMovieSceneDataChanged()
{
}
void FGAEffectCueEditor::ChangeViewedAnimation(UGAEffectCueSequence& InAnimationToView)
{
}
FGAEffectCueEditor::~FGAEffectCueEditor()
{
	FEditorDelegates::OnAssetPostImport.RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);
	FBlueprintEditorModule* BlueprintEditorModule = &FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule->OnRegisterTabsForEditor().Remove(BlueprintEditorTabSpawnerHandle);
	BlueprintEditorModule->OnRegisterLayoutExtensions().Remove(BlueprintEditorLayoutExtensionHandle);
	BlueprintEditorTabSpawnerHandle.Reset();
	BlueprintEditorLayoutExtensionHandle.Reset();
	TabManager.Pin()->UnregisterTabSpawner(FName("EmbeddedEffectCueSequenceID"));
	// NOTE: Any tabs that we still have hanging out when destroyed will be cleaned up by FBaseToolkit's destructor
}

void FGAEffectCueEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	TabManager = InTabManager;
	FBlueprintEditor::RegisterTabSpawners(InTabManager);
}
void FGAEffectCueEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FGAEffectCueEditor::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const FName AppIdentifier, const TSharedRef<FTabManager::FLayout>& StandaloneDefaultLayout, const bool bCreateDefaultStandaloneMenu, const bool bCreateDefaultToolbar, const TArray<UObject*>& ObjectsToEdit, const bool bInIsToolbarFocusable)
{
	
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit, bInIsToolbarFocusable);
}
void FGAEffectCueEditor::InitEffectCueEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
	UBlueprint* BP = InBlueprints[0];
	EditedCue = BP->GeneratedClass->GetDefaultObject<AAFCueActor>();
	CueClass = BP->GeneratedClass;

	for (auto Blueprint : InBlueprints)
	{
		EnsureAbilityBlueprintIsUpToDate(Blueprint);
	}
}

void FGAEffectCueEditor::EnsureAbilityBlueprintIsUpToDate(UBlueprint* Blueprint)
{
#if WITH_EDITORONLY_DATA
	int32 Count = Blueprint->UbergraphPages.Num();
	for (auto Graph : Blueprint->UbergraphPages)
	{
		// remove the default event graph, if it exists, from existing Gameplay Ability Blueprints
		if (Graph->GetName() == "EventGraph" && Graph->Nodes.Num() == 0)
		{
			check(!Graph->Schema->GetClass()->IsChildOf(UGAEffectCueGraphSchema::StaticClass()));
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
			break;
		}
	}
#endif
}
bool FGAEffectCueEditor::IsBlueprintEditor() const
{ 
	return true;
}

FName FGAEffectCueEditor::GetToolkitFName() const
{
	return FName("EffectCueEditor");
}

FText FGAEffectCueEditor::GetBaseToolkitName() const
{
	return FText::FromString("Game Effect Cue");
}

FText FGAEffectCueEditor::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	check(EditingObjs.Num() > 0);

	FFormatNamedArguments Args;

	const UObject* EditingObject = EditingObjs[0];

	const bool bDirtyState = EditingObject->GetOutermost()->IsDirty();

	Args.Add(TEXT("ObjectName"), FText::FromString(EditingObject->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(FText::FromString("{ObjectName}{DirtyState}"), Args);
}

FText FGAEffectCueEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetEditingObject();

	check (EditingObject != NULL);

	return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
}

FString FGAEffectCueEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("EffectCueEditor");
}

FLinearColor FGAEffectCueEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

UBlueprint* FGAEffectCueEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UAFCueActorBlueprint>())
		{ 
			return (UBlueprint*)EditingObjs[i]; 
		}
	}
	return nullptr;
}

FString FGAEffectCueEditor::GetDocumentationLink() const
{
	return FBlueprintEditor::GetDocumentationLink(); // todo: point this at the correct documentation
}

#undef LOCTEXT_NAMESPACE

