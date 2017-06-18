// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Kismet/Public/BlueprintEditor.h"

//////////////////////////////////////////////////////////////////////////
// FGameplayAbilitiesEditor

/**
 * Gameplay abilities asset editor (extends Blueprint editor)
 */
class FGAEffectEditor : public FBlueprintEditor
{
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

	void InitEffectEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);
public:
	FGAEffectEditor();

	virtual ~FGAEffectEditor();

public:
	// IToolkit interface
	// FRED_TODO: don't merge this back
//	virtual FName GetToolkitContextFName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsAssetEditor() const override { return true; }
	virtual bool IsBlueprintEditor() const override { return false; };
	virtual void PostRegenerateMenusAndToolbars() override { RemoveAllToolbarWidgets(); };
	// End of IToolkit interface

	/** @return the documentation location for this editor */
	virtual FString GetDocumentationLink() const override;
	
	/** Returns a pointer to the Blueprint object we are currently editing, as long as we are editing exactly one */
	virtual UBlueprint* GetBlueprintObj() const override;
};
