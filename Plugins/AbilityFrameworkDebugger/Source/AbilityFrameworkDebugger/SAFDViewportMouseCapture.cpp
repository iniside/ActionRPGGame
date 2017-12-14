// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDViewportMouseCapture.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDViewportMouseCapture::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SAFDViewportMouseCapture::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnMouseButtonDownDelegate.ExecuteIfBound();
	return FReply::Handled();
}