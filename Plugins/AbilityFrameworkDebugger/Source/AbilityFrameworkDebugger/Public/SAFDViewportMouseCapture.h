// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API SAFDViewportMouseCapture : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDViewportMouseCapture)
	{}
	SLATE_END_ARGS()

	FSimpleDelegate OnMouseButtonDownDelegate;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};
