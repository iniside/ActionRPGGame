// Fill out your copyright notice in the Description page of Project Settings.

#include "SARDrawTestWidget.h"
#include "Rendering/DrawElements.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARDrawTestWidget::Construct(const FArguments& InArgs)
{

	ProgressStep = 0.25f;
	Progress = 0.0f;
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
SARDrawTestWidget::SARDrawTestWidget()
{
	Brush = nullptr;
}
int32 SARDrawTestWidget::OnPaint(const FPaintArgs& Args
	, const FGeometry& AllottedGeometry
	, const FSlateRect& MyCullingRect
	, FSlateWindowElementList& OutDrawElements
	, int32 LayerId
	, const FWidgetStyle& InWidgetStyle
	, bool bParentEnabled) const
{
	/*TArray<FVector2D> PointsUp;
	PointsUp.Add(FVector2D(10, 0));
	PointsUp.Add(FVector2D(20, 0));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsUp, ESlateDrawEffect::None, FLinearColor::White, true, 2);
	
	TArray<FVector2D> PointsDown;
	PointsDown.Add(FVector2D(-10, 0));
	PointsDown.Add(FVector2D(-20, 0));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsDown, ESlateDrawEffect::None, FLinearColor::White, true, 2);

	TArray<FVector2D> PointsLeft;
	PointsLeft.Add(FVector2D(0, 10));
	PointsLeft.Add(FVector2D(0, 20));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsLeft, ESlateDrawEffect::None, FLinearColor::White, true, 2);


	TArray<FVector2D> PointsRight;
	PointsRight.Add(FVector2D(0, -10));
	PointsRight.Add(FVector2D(0, -20));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsRight, ESlateDrawEffect::None, FLinearColor::White, true, 2);*/


	float X = FMath::Sin(FMath::DegreesToRadians(60.0f));
	float Y = FMath::Cos(FMath::DegreesToRadians(60.0f));
	FVector2D Dir(X, Y);
	TArray<FVector2D> PointsRight;
	PointsRight.Add(Dir*10);
	
	
	PointsRight.Add(Dir*30);
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsRight, ESlateDrawEffect::None, FLinearColor::White, true, 2);

	float X2 = FMath::Sin(FMath::DegreesToRadians(-60.0f));
	float Y2 = FMath::Cos(FMath::DegreesToRadians(-60.0f));
	FVector2D Dir2(X2, Y2);
	TArray<FVector2D> PointsLeft;
	PointsLeft.Add(FVector2D(Dir2 * 10));
	PointsLeft.Add(Dir2 * 30);
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsLeft, ESlateDrawEffect::None, FLinearColor::White, true, 2);

	float X3 = FMath::Sin(FMath::DegreesToRadians(180.0f));
	float Y3 = FMath::Cos(FMath::DegreesToRadians(180.0f));
	FVector2D Dir3(X3, Y3);
	TArray<FVector2D> PointsTop;
	PointsTop.Add(FVector2D(Dir3 * 10));
	PointsTop.Add(Dir3 * 30);
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), PointsTop, ESlateDrawEffect::None, FLinearColor::White, true, 2);

	if (Brush)
	{
		FSlateLayoutTransform Transform(FVector2D(0, 0));
		FSlateDrawElement::MakeBox(OutDrawElements
			, LayerId
			, AllottedGeometry.ToPaintGeometry(FVector2D(128,128), Transform)
			, Brush);
	}

	
	float Step = 3.0f;
	int32 Steps = 30;

	//{
	//	TArray<FVector2D> CirclePoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step) - 120);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step) - 120);
	//		FVector2D CDir(XC, YC);
	//		CirclePoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CirclePoints, ESlateDrawEffect::None, FLinearColor::White, true, 12);
	//}
	//{
	//	TArray<FVector2D> ProgressPoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step *Progress) - 120);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step *Progress) - 120);
	//		FVector2D CDir(XC, YC);
	//		ProgressPoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(), ProgressPoints, ESlateDrawEffect::None, FLinearColor::Red, true, 12);
	//}

	//{
	//	TArray<FVector2D> CirclePoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step) + 120);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step) + 120);
	//		FVector2D CDir(XC, YC);
	//		CirclePoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	/*	FPaintGeometry PG = AllottedGeometry.ToPaintGeometry();
	//	FSlateRenderTransform RT;

	//	PG.AppendTransform()*/
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CirclePoints, ESlateDrawEffect::None, FLinearColor::White, true, 12);
	//}
	//{
	//	TArray<FVector2D> ProgressPoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step *Progress) + 120);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step *Progress) + 120);
	//		FVector2D CDir(XC, YC);
	//		ProgressPoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(), ProgressPoints, ESlateDrawEffect::None, FLinearColor::Red, true, 12);
	//}
	
	//{
	//	TArray<FVector2D> CirclePoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step) + 20);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step) + 20);
	//		FVector2D CDir(XC, YC);
	//		CirclePoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	/*	FPaintGeometry PG = AllottedGeometry.ToPaintGeometry();
	//	FSlateRenderTransform RT;

	//	PG.AppendTransform()*/
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CirclePoints, ESlateDrawEffect::None, FLinearColor::White, true, 12);
	//}
	//{
	//	TArray<FVector2D> ProgressPoints;
	//	for (int32 Idx = 0; Idx < Steps; Idx++)
	//	{
	//		float XC = FMath::Sin(FMath::DegreesToRadians(Idx*Step *Progress) + 20);
	//		float YC = FMath::Cos(FMath::DegreesToRadians(Idx*Step *Progress) + 20);
	//		FVector2D CDir(XC, YC);
	//		ProgressPoints.Add(FVector2D(CDir*40.0f));
	//	}
	//	FSlateDrawElement::MakeLines(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(), ProgressPoints, ESlateDrawEffect::None, FLinearColor::Red, true, 12);
	//}

	return LayerId;
}
void SARDrawTestWidget::Tick(const FGeometry& AllottedGeometry
	, const double InCurrentTime
	, const float InDeltaTime)
{
	if (Progress > 1.0f)
	{
		Progress = 0;
	}
	else
	{
		Progress += (ProgressStep*InDeltaTime);
	}
}
