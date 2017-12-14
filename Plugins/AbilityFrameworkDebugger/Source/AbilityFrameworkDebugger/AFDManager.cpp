// Fill out your copyright notice in the Description page of Project Settings.

#include "AFDManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate.h"
#include "SlateCore.h"

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

	WindowDesktop = SNew(SDraggableDesktopWidget).Visibility(EVisibility::SelfHitTestInvisible);
	GEngine->GameViewport->AddViewportWidgetContent(WindowDesktop.ToSharedRef());
}

FDWWWindowHandle FAFDManager::AddNewWindow(TSharedPtr<SDraggableWindowWidget> InWindow)
{
	return WindowDesktop->AddWindow(InWindow);
}