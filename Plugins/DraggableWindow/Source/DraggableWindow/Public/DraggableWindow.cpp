// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DraggableWindow.h"
#include "DWManager.h"
#if WITH_EDITORONLY_DATA
#include "Editor.H"
#endif
#define LOCTEXT_NAMESPACE "FDraggableWindowModule"

void FDraggableWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

#if WITH_EDITORONLY_DATA
	FEditorDelegates::EndPIE.AddRaw(this, &FDraggableWindowModule::HandlePIEEnd);
#endif //WITH_EDITORONLY_DATA

}

void FDraggableWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
#if WITH_EDITORONLY_DATA
void FDraggableWindowModule::HandlePIEEnd(bool InVal)
{
	FDWManager::PIEDestroy();
}
#endif
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDraggableWindowModule, DraggableWindow)