// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBlueprintFunctionLibrary.h"

UGAAttributesBlueprintFunctionLibrary::UGAAttributesBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FName UGAAttributesBlueprintFunctionLibrary::GetAttribute(FGAAttribute AttributeIn)
{
	return AttributeIn.AttributeName;
}