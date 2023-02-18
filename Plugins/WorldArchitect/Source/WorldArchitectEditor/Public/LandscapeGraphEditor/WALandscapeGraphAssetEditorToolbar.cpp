#include "WALandscapeGraphAssetEditorToolbar.h"
#include "WALandscapeGraphAssetEditor.h"
#include "WALandscapeGraphEditorCommands.h"
#include "MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "GenericGraphAssetEditorToolbar"

void FWALandscapeGraphAssetEditorToolbar::AddGenericGraphToolbar(TSharedPtr<FExtender> Extender)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FWALandscapeGraphAssetEditor> GenericGraphEditorPtr = GenericGraphEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, GenericGraphEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP( this, &FWALandscapeGraphAssetEditorToolbar::FillGenericGraphToolbar ));
	GenericGraphEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FWALandscapeGraphAssetEditorToolbar::FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FWALandscapeGraphAssetEditor> GenericGraphEditorPtr = GenericGraphEditor.Pin();

	ToolbarBuilder.BeginSection("Landscape Graph");
	{
		{
			const FText GraphSettingsLabel = LOCTEXT("GraphSettings_Label", "Graph Settings");
			const FText GraphSettingsTip = LOCTEXT("GraphSettings_ToolTip", "Show the Graph Settings");
			const FSlateIcon GraphSettingsIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.GameSettings");
			ToolbarBuilder.AddToolBarButton(FWALandscapeGraphEditorCommands::Get().GraphSettings,
				NAME_None,
				GraphSettingsLabel,
				GraphSettingsTip,
				GraphSettingsIcon);
		}
		{
			const FText GraphSettingsLabel = LOCTEXT("GraphSettings_Label", "Generate Landscape");
			const FText GraphSettingsTip = LOCTEXT("GraphSettings_ToolTip", "enerate LAndscape from Current graph");
			const FSlateIcon GraphSettingsIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.GameSettings");
			ToolbarBuilder.AddToolBarButton(FWALandscapeGraphEditorCommands::Get().GenerateLandscape,
				NAME_None,
				GraphSettingsLabel,
				GraphSettingsTip,
				GraphSettingsIcon);
		}
	}
	ToolbarBuilder.EndSection();

}


#undef LOCTEXT_NAMESPACE
