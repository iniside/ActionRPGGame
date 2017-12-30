// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDraggableWindowWidget.h"

/**
 * 
 */
class DRAGGABLEWINDOW_API FDWManager
{
public:
	friend class SDraggableWindowWidget;
	friend class SDraggableDesktopWidget;
private:
	static FDWManager* Instance;
	TSharedPtr<SDraggableDesktopWidget> Desktop;
	TSet<FDWWWindowHandle> WindowHandles;
	void Init();
	void CleanUp();
	void RemoveWindow(const FDWWWindowHandle& InHandle);
	FDWManager();
public:
	static FDWManager& Get();
#if WITH_EDITORONLY_DATA
	static void PIEDestroy();
#endif //WITH_EDITORONLY_DATA
	~FDWManager();

	FDWWWindowHandle CreateWindow();
	FDWWWindowHandle CreateWindow(TSharedPtr<SWidget> InWindowContent);
	FDWWWindowHandle AddWindow(TSharedPtr<SDraggableWindowWidget> InWindowWidget);
};
typedef FDWManager FDraggableWindowManager;