// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WorldArchitectEdMode.h"
#include "WorldArchitectEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FWorldArchitectEdMode::EM_WorldArchitectEdModeId = TEXT("EM_WorldArchitectEdMode");

FWorldArchitectEdMode::FWorldArchitectEdMode()
{

}

FWorldArchitectEdMode::~FWorldArchitectEdMode()
{

}

void FWorldArchitectEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FWorldArchitectEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FWorldArchitectEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FWorldArchitectEdMode::UsesToolkits() const
{
	return true;
}




