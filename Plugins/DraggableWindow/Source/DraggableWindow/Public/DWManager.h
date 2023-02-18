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
	//name of window, window handle
	TMap<FName, FDWWWindowHandle> WindowsByName;
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

	FDWWWindowHandle CreateWindow(const FString& WindowName);
	FDWWWindowHandle CreateWindow(TSharedPtr<SWidget> InWindowContent, const FString& WindowName);
	FDWWWindowHandle AddWindow(TSharedPtr<SDraggableWindowWidget> InWindowWidget, const FString& WindowName);
};
typedef FDWManager FDraggableWindowManager;