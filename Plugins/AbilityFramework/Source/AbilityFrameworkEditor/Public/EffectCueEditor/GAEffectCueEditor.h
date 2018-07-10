// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "GraphEditor.h"
#include "ISequencer.h"
#include "Effects/GAEffectCueSequence.h"
#include "Editor/Kismet/Public/BlueprintEditor.h"
#include "WorkflowTabFactory.h"
//////////////////////////////////////////////////////////////////////////
// FGameplayAbilitiesEditor
class UGAEffectCueSequence;
/**
 * Gameplay abilities asset editor (extends Blueprint editor)
 */

class FGAEffectCueEditor : public FBlueprintEditor
{
	TWeakPtr<FTabManager> TabManager;
	FDelegateHandle OnSequenceChangedHandle;

	TArray<UObject*> GetAnimationEventContexts() const;
	FDelegateHandle BlueprintEditorTabSpawnerHandle, BlueprintEditorLayoutExtensionHandle;
	class AAFCueActor* EditedCue;
	TSubclassOf<AAFCueActor> CueClass;
	TWeakObjectPtr<class UGAEffectCueSequence> EditedSequence;
	void ChangeViewedAnimation(UGAEffectCueSequence& InAnimationToView);
	void OnMovieSceneDataChanged();
	void OnSequenceChanged();
	void RegisterBlueprintEditorLayout(FLayoutExtender& Extender);
	void RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor);
public:
	virtual void InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const FName AppIdentifier, const TSharedRef<FTabManager::FLayout>& StandaloneDefaultLayout, const bool bCreateDefaultStandaloneMenu, const bool bCreateDefaultToolbar, const TArray<UObject*>& ObjectsToEdit, const bool bInIsToolbarFocusable = false) override;
	/**
	 * Edits the specified gameplay ability asset(s)
	 *
	 * @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	 * @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	 * @param	InBlueprints			The blueprints to edit
	 * @param	bShouldOpenInDefaultsMode	If true, the editor will open in defaults editing mode
	 */ 

	void InitEffectCueEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);
private:
	/**
	 * Updates existing gameplay ability blueprints to make sure that they are up to date
	 * 
	 * @param	Blueprint	The blueprint to be updated
	 */
	void EnsureAbilityBlueprintIsUpToDate(UBlueprint* Blueprint);

public:
	FGAEffectCueEditor();

	virtual ~FGAEffectCueEditor();

public:
	// IToolkit interface
	// FRED_TODO: don't merge this back
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsBlueprintEditor() const override;
	// End of IToolkit interface

	/** @return the documentation location for this editor */
	virtual FString GetDocumentationLink() const override;
	
	/** Returns a pointer to the Blueprint object we are currently editing, as long as we are editing exactly one */
	virtual UBlueprint* GetBlueprintObj() const override;
};
