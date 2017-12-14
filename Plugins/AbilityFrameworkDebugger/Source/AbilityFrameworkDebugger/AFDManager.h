// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAFDDesktopWidget.h"
/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API FAFDManager
{
private:
	static FAFDManager* Instance;
	TSharedPtr<SAFDDesktopWidget> Dekstop;
	TSharedPtr<SDraggableDesktopWidget> WindowDesktop;
protected:
	void Init();
public:

	static FAFDManager& Get()
	{
		if(!Instance)
		{
			Instance = new FAFDManager();
			Instance->Init();
		}
		return*Instance;
	}

	FDWWWindowHandle AddNewWindow(TSharedPtr<SDraggableWindowWidget> InWindow);

	FAFDManager();
	~FAFDManager();
};
