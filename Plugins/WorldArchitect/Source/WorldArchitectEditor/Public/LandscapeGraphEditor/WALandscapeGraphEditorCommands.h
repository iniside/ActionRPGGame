#pragma once

class FWALandscapeGraphEditorCommands : public TCommands<FWALandscapeGraphEditorCommands>
{
public:
	/** Constructor */
	FWALandscapeGraphEditorCommands()
		: TCommands<FWALandscapeGraphEditorCommands>("GenericGraphEditor", NSLOCTEXT("Contexts", "GenericGraphEditor", "Generic Graph Editor"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}
	
	TSharedPtr<FUICommandInfo> GraphSettings;
	
	TSharedPtr<FUICommandInfo> GenerateLandscape;

	virtual void RegisterCommands() override;
};
