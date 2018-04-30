
#pragma once

#include "CoreMinimal.h"

class FWALandscapeGraphAssetEditor;
class FExtender;
class FToolBarBuilder;

class FWALandscapeGraphAssetEditorToolbar : public TSharedFromThis<FWALandscapeGraphAssetEditorToolbar>
{
public:
	FWALandscapeGraphAssetEditorToolbar(TSharedPtr<FWALandscapeGraphAssetEditor> InGenericGraphEditor)
		: GenericGraphEditor(InGenericGraphEditor) {}

	//void AddModesToolbar(TSharedPtr<FExtender> Extender);
	//void AddDebuggerToolbar(TSharedPtr<FExtender> Extender);
	void AddGenericGraphToolbar(TSharedPtr<FExtender> Extender);

private:
	//void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);
	//void FillDebuggerToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FWALandscapeGraphAssetEditor> GenericGraphEditor;
};
