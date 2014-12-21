// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"

#include "IGAAttributes.h"
#include "GAAttributesBase.h"

#include "GAAttributesBlueprintFunctionLibrary.h"

UGAAttributesBlueprintFunctionLibrary::UGAAttributesBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FName UGAAttributesBlueprintFunctionLibrary::GetAttribute(FGAAttribute AttributeIn)
{
	return AttributeIn.AttributeName;
}
float UGAAttributesBlueprintFunctionLibrary::GetAttributeFloat(AActor* Target, FGAAttribute AttributeIn)
{
	IIGAAttributes* attributeInt = Cast<IIGAAttributes>(Target);
	if (!attributeInt)
		return 0;

	return  attributeInt->GetAttributes()->GetFloatValue(AttributeIn);
}
float UGAAttributesBlueprintFunctionLibrary::ChangeAttribute(AActor* Target, FGAAttribute AttributeIn, float ValueIn, EGAAttributeOp Operation, bool bSetAttribute)
{
	//check if actor have interface, if not, then this actor can't interact with attribute system.
	IIGAAttributes* attributeInt = Cast<IIGAAttributes>(Target);
	if (!attributeInt)
	{
		/*
			Probabaly should print some message, like 
			you need to implement proper interface to interact with this system
			and add component.
		*/
		return 0;
	}
	//shouldn't be null if we are past interface stage..
	UGAAttributesBase* attributes = attributeInt->GetAttributes();

	float newValue = attributes->AttributeOperation(AttributeIn, ValueIn, Operation);
	if (bSetAttribute)
	{
		attributes->SetFloatValue(AttributeIn, newValue);
	}

	return newValue;
}