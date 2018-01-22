// Fill out your copyright notice in the Description page of Project Settings.

#include "AFDManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate.h"
#include "SlateCore.h"
#include "DWManager.h"

FAFDManager* FAFDManager::Instance = nullptr;

FAFDManager::FAFDManager()
{
}

FAFDManager::~FAFDManager()
{
}

void FAFDManager::Init()
{
	Dekstop = SNew(SAFDDesktopWidget).Visibility(EVisibility::SelfHitTestInvisible);
	GEngine->GameViewport->AddViewportWidgetContent(Dekstop.ToSharedRef());
}

FDWWWindowHandle FAFDManager::AddDebugWindow(TSharedPtr<SWidget> InWindowContent)
{
	return FDWManager::Get().CreateWindow(InWindowContent, "Debug");
}

#if WITH_EDITORONLY_DATA
void FAFDManager::PIEDestroy()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
#endif