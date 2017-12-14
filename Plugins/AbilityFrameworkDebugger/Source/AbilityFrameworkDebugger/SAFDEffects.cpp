// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDEffects.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDEffects::Construct(const FArguments& InArgs)
{
	
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SNew(SButton)
		]
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
