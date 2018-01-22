// Fill out your copyright notice in the Description page of Project Settings.

#include "DWManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

FDWManager* FDWManager::Instance = nullptr;
void FDWManager::Init()
{
	Desktop = SNew(SDraggableDesktopWidget).Visibility(EVisibility::SelfHitTestInvisible);
	GEngine->GameViewport->AddViewportWidgetContent(Desktop.ToSharedRef(), 2000);
}
void FDWManager::CleanUp()
{
	Desktop->Clean();
	Desktop.Reset();
	WindowHandles.Empty();
	WindowHandles.Shrink();
}
void FDWManager::RemoveWindow(const FDWWWindowHandle& InHandle)
{
	Desktop->RemoveWindow(InHandle.Window.Pin());
	WindowHandles.Remove(InHandle);
}
FDWManager& FDWManager::Get()
{
	if (!Instance)
	{
		Instance = new FDWManager();
		Instance->Init();
	}
	return *Instance;
}
#if WITH_EDITORONLY_DATA
void FDWManager::PIEDestroy()
{
	if (Instance)
	{
		Instance->CleanUp();
		delete Instance;
		Instance = nullptr;
	}
}
#endif //WITH_EDITORONLY_DATA
FDWManager::FDWManager()
{
}

FDWManager::~FDWManager()
{
}

FDWWWindowHandle FDWManager::CreateWindow(const FString& WindowName)
{
	TSharedPtr<SDraggableWindowWidget> NewWindow = SNew(SDraggableWindowWidget);
	return AddWindow(NewWindow, WindowName);
}
FDWWWindowHandle FDWManager::CreateWindow(TSharedPtr<SWidget> InWindowContent, const FString& WindowName)
{
	
	TSharedPtr<SDraggableWindowWidget> NewWindow = SNew(SDraggableWindowWidget);
	NewWindow->AddContent(InWindowContent);
	return AddWindow(NewWindow, WindowName);
}
FDWWWindowHandle FDWManager::AddWindow(TSharedPtr<SDraggableWindowWidget> InWindowWidget, const FString& WindowName)
{
	FDWWWindowHandle Handle = Desktop->AddWindow(InWindowWidget);
	WindowHandles.Add(Handle);
	WindowsByName.Add(FName(*WindowName), Handle);
	return Handle;
}