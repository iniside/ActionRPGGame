// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SDraggableWindowWidget.h"
#include "SWidgetSwitcher.h"

class ABILITYFRAMEWORKDEBUGGER_API SAFDMainWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDMainWidget)
	{}
	SLATE_END_ARGS()
protected:
	TWeakObjectPtr<UWorld> World;
	TWeakObjectPtr<AActor> SelectedActor;
	TSharedPtr<SWidgetSwitcher> Content;
	FDWWWindowHandle WindowHandle;
	TSharedPtr<class SAFDViewportMouseCapture> CaptureWidget;
	/** Constructs this widget with InArgs */
public:
	void Construct(const FArguments& InArgs);
protected:
	FReply OnPickActorClicked();
	FReply OnAttributesClicked();
	FReply OnEffectsClicked();

	void OnActorPicked();

	FText GetActorName() const;
public:
	void SetWindowHandle(const FDWWWindowHandle& InHandle)
	{
		WindowHandle = InHandle;
	}
};

/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API SAFDDesktopWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDDesktopWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FReply OnNewDebugWindowClicked();
};
