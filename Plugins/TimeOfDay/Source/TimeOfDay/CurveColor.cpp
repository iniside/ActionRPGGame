// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDay.h"
#include "CurveColor.h"

//FLinearColor FRuntimeCurveLinearColor::GetLinearColorValue(float InTime) const
//{
//	FLinearColor Result;
//
//	Result.R = ColorCurves[0].Eval(InTime);
//	Result.G = ColorCurves[1].Eval(InTime);
//	Result.B = ColorCurves[2].Eval(InTime);
//
//	// No alpha keys means alpha should be 1
//	if (ColorCurves[3].GetNumKeys() == 0)
//	{
//		Result.A = 1.0f;
//	}
//	else
//	{
//		Result.A = ColorCurves[3].Eval(InTime);
//	}
//
//	return Result;
//}