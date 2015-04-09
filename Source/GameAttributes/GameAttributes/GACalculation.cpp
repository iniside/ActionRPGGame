// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GACalculation.h"

UGACalculation::UGACalculation(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
FGAAttributeData UGACalculation::OutgoingModifyEffect(const FGAAttributeData& DataIn)
{
	FGAAttributeData data = DataIn;
	return data;
}
FGAAttributeData UGACalculation::IncomingModifyEffect(const FGAAttributeData& DataIn)
{
	FGAAttributeData data = DataIn;
	return data;
}