// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IDetailCustomization.h"
#include "PropertyHandle.h"

#include "GraphEditor.h"
#include "ISequencer.h"
#include "Effects/GAEffectCueSequence.h"
#include "Editor/Kismet/Public/BlueprintEditor.h"
#include "WorkflowTabFactory.h"

struct FEffectCueSequenceEditorSummoner
	: public FWorkflowTabFactory
{
	FEffectCueSequenceEditorSummoner(TSharedPtr<FBlueprintEditor> BlueprintEditor);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	/*virtual TSharedRef<SDockTab> SpawnTab(const FWorkflowTabSpawnInfo& Info) const override;*/
protected:
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor;
};
class FAFEffectCueDetails : public IDetailCustomization
{
protected:
	TSharedPtr<ISequencer> Sequencer;
	FDelegateHandle OnBlueprintPreCompileHandle;
	TSharedPtr<SDockTab> Tab;
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();
	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};