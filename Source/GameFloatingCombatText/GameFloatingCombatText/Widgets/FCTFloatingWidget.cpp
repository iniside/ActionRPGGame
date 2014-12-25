// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameFloatingCombatText.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "../FCTFloatingTextComponent.h"

#include "FCTFloatingWidget.h"

UFCTFloatingWidget::UFCTFloatingWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//void UFCTFloatingWidget::Tick_Implementation(FGeometry MyGeometry, float InDeltaTime)
//{
//	Super::Tick(MyGeometry, InDeltaTime);
//}
//void UFCTFloatingWidget::Construct_Implementation()
//{
//	Super::Construct();
//	SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
//}
void UFCTFloatingWidget::MyTick(float DeltaTimeIn)
{
	if (LifeTime > 0)
	{
		//FVector2D ScreenLocation;
		SetVisibility(ESlateVisibility::HitTestInvisible);
		//PCOwner->ProjectWorldLocationToScreen(DisplayData.TargetLocation, ScreenLocation);

		//SetPositionInViewport(ScreenLocation);
		LifeTime -= DeltaTimeIn;
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}