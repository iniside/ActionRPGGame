// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"

#include "IGAAttributes.h"
#include "GAAttributesBase.h"
#include "GAAttributeComponent.h"

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

void UGAAttributesBlueprintFunctionLibrary::AttributesOperation(TArray<FGAAttributeModifier> AttributesIn)
{
	for (const FGAAttributeModifier& attributeMod : AttributesIn)
	{
		IIGAAttributes* attributeInt = Cast<IIGAAttributes>(attributeMod.Target.Get());
		if (!attributeInt)
		{
			return;
		}
		UGAAttributesBase* attributes = attributeInt->GetAttributes();
		float newValue = attributes->AttributeOperation(attributeMod.Attribute, attributeMod.Value, attributeMod.Operation);
		//if (bSetAttribute)
		//{
		attributes->SetFloatValue(attributeMod.Attribute, newValue);
		//}
	}
}

void UGAAttributesBlueprintFunctionLibrary::ModifyAttributes(TArray<FGAAttributeModifier> AttributesIn)
{
	for (FGAAttributeModifier& attributeMod : AttributesIn)
	{
		IIGAAttributes* instAttr = Cast<IIGAAttributes>(attributeMod.Instigator.Get());
		IIGAAttributes* targetAttr = Cast<IIGAAttributes>(attributeMod.HitTarget.Actor.Get());
		if (!instAttr || !targetAttr)
		{
			return;
		}
		UGAAttributeComponent* attributes = instAttr->GetAttributeComponent();
		//float newValue = attributes->AttributeOperation(attributeMod.Attribute, attributeMod.Value, attributeMod.Operation);
		//if (bSetAttribute)
		//{
		attributes->ModifyAttributesOnTarget(targetAttr->GetAttributeComponent(), attributeMod);
		//}
	}
}