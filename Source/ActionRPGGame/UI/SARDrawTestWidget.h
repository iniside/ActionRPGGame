// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ACTIONRPGGAME_API SARDrawTestWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SARDrawTestWidget)
	{}
	SLATE_END_ARGS()
	SARDrawTestWidget();
	float Progress;
	float ProgressStep;
	FSlateBrush* Brush;
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args
		, const FGeometry& AllottedGeometry
		, const FSlateRect& MyCullingRect
		, FSlateWindowElementList& OutDrawElements
		, int32 LayerId
		, const FWidgetStyle& InWidgetStyle
		, bool bParentEnabled) const override;

	virtual void Tick(const FGeometry& AllottedGeometry
		, const double InCurrentTime
		, const float InDeltaTime) override;
};
