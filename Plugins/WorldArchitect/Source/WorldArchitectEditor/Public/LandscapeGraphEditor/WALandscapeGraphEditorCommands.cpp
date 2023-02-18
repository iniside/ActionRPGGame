
#include "WALandscapeGraphEditorCommands.h"

#define LOCTEXT_NAMESPACE "GenericGraphEditorCommands"

void FWALandscapeGraphEditorCommands::RegisterCommands()
{
	UI_COMMAND(GraphSettings, "Graph Settings", "Graph Settings", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(GenerateLandscape, "Generate Landscape", "Generate Landscape", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
