// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "../AbilityFrameworkEditor.h"
#include "GAEffectEditor.h"
#include "EditorReimportHandler.h"

#if WITH_EDITOR
#include "Editor.h"
#endif
//Editor\UnrealEd\Public\Toolkits\ToolkitManager.h
//\Editor\UnrealEd\Private\Toolkits\SStandaloneAssetEditorToolkitHost.h
#include "Editor/UnrealEd/Private/Toolkits/SStandaloneAssetEditorToolkitHost.h"
#include "Toolkits/ToolkitManager.h"
//#include "Toolkits/AssetEditorCommonCommands.h"
#include "Toolkits/GlobalEditorCommonCommands.h"

#include "Effects/GAEffectBlueprint.h"
#include "GAEffectBlueprintFactory.h"
#include "GAEffectGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FGAEffectEditor"


/////////////////////////////////////////////////////
// FGameplayAbilitiesEditor

FGAEffectEditor::FGAEffectEditor()
{
	// todo: Do we need to register a callback for when properties are changed?
	//bCreateMenuExtenders = true;
	//bCreateDefaultStandaloneMenu = false;
	//bCreateDefaultToolbar = false;
}

FGAEffectEditor::~FGAEffectEditor()
{
	FEditorDelegates::OnAssetPostImport.RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);

	// NOTE: Any tabs that we still have hanging out when destroyed will be cleaned up by FBaseToolkit's destructor
}
void FGAEffectEditor::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const FName AppIdentifier, const TSharedRef<FTabManager::FLayout>& StandaloneDefaultLayout, const bool bCreateDefaultStandaloneMenu, const bool bCreateDefaultToolbar, const TArray<UObject*>& ObjectsToEdit, const bool bInIsToolbarFocusable)
{
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, StandaloneDefaultLayout, true, true, ObjectsToEdit, true);
}
void FGAEffectEditor::InitEffectEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
	RemoveAllToolbarWidgets();
}


// FRED_TODO: don't merge this back
// FName FGameplayAbilitiesEditor::GetToolkitContextFName() const
// {
// 	return GetToolkitFName();
// }

FName FGAEffectEditor::GetToolkitFName() const
{
	return FName("GameEffectEditor");
}

FText FGAEffectEditor::GetBaseToolkitName() const
{
	return FText::FromString("Game Effect Editor");
}

FText FGAEffectEditor::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	check(EditingObjs.Num() > 0);

	FFormatNamedArguments Args;

	const UObject* EditingObject = EditingObjs[0];

	const bool bDirtyState = EditingObject->GetOutermost()->IsDirty();

	Args.Add(TEXT("ObjectName"), FText::FromString(EditingObject->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("GameplayAbilitiesToolkitName", "{ObjectName}{DirtyState}"), Args);
}

FText FGAEffectEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetEditingObject();

	check(EditingObject != NULL);

	return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
}

FString FGAEffectEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("EffectEditor");
}

FLinearColor FGAEffectEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

UBlueprint* FGAEffectEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UGAEffectBlueprint>())
		{
			return (UBlueprint*)EditingObjs[i];
		}
	}
	return nullptr;
}

FString FGAEffectEditor::GetDocumentationLink() const
{
	return FBlueprintEditor::GetDocumentationLink(); // todo: point this at the correct documentation
}

#undef LOCTEXT_NAMESPACE

