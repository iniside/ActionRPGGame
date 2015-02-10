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
bool UGAAttributesBlueprintFunctionLibrary::EqualAttribute(const FGAAttribute& Compare, FGAAttribute Against)
{
	return Compare == Against;
}
FName UGAAttributesBlueprintFunctionLibrary::GetAttribute(FGAAttribute AttributeIn)
{
	return AttributeIn.AttributeName;
}
float UGAAttributesBlueprintFunctionLibrary::GetFinalAttributeValue(AActor* Target, FGAAttribute Name)
{
	IIGAAttributes* attributeInt = Cast<IIGAAttributes>(Target);
	if (!attributeInt)
		return 0;
	return attributeInt->GetAttributes()->GetFinalAttributeValue(Name);
}
float UGAAttributesBlueprintFunctionLibrary::GetCurrentAttributeValue(AActor* Target, FGAAttribute Name)
{
	IIGAAttributes* attributeInt = Cast<IIGAAttributes>(Target);
	if (!attributeInt)
		return 0;
	return attributeInt->GetAttributes()->GetCurrentAttributeValue(Name);
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
	//	/*
	//		Probabaly should print some message, like 
	//		you need to implement proper interface to interact with this system
	//		and add component.
	//	*/
	//	return 0;
	}
	////shouldn't be null if we are past interface stage..
	UGAAttributesBase* attributes = attributeInt->GetAttributes();
	float newVal = attributes->AttributeOperation(AttributeIn, ValueIn, Operation);
	FGAEvalData eval;
	eval.Attribute = AttributeIn;
	eval.ModValue = newVal;
	attributes->UpdateAttributes(eval, newVal);
	return 0;
}

void UGAAttributesBlueprintFunctionLibrary::AttributesOperation(TArray<FGAAttributeModSelf> AttributesIn)
{
	//for (const FGAAttributeModSelf& attributeMod : AttributesIn)
	//{
	//	IIGAAttributes* attributeInt = Cast<IIGAAttributes>(attributeMod.Target.Get());
	//	if (!attributeInt)
	//	{
	//		return;
	//	}
	//	UGAAttributesBase* attributes = attributeInt->GetAttributes();
	//	float newValue = attributes->AttributeOperation(attributeMod.Attribute, attributeMod.Value, attributeMod.Operation);
	//	//if (bSetAttribute)
	//	//{
	//	attributes->SetFloatValue(attributeMod.Attribute, newValue);
	//	//}
	//}
}

void UGAAttributesBlueprintFunctionLibrary::ModifyAttributes(TArray<FGAAttributeModData> AttributesIn)
{
	//for (FGAAttributeModifier& attributeMod : AttributesIn)
	//{
	//	IIGAAttributes* instAttr = Cast<IIGAAttributes>(attributeMod.Instigator.Get());
	//	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(attributeMod.HitTarget.Actor.Get());
	//	if (!instAttr || !targetAttr)
	//	{
	//		return;
	//	}
	//	UGAAttributeComponent* attributes = instAttr->GetAttributeComponent();
	//	//float newValue = attributes->AttributeOperation(attributeMod.Attribute, attributeMod.Value, attributeMod.Operation);
	//	//if (bSetAttribute)
	//	//{
	//	attributes->ModifyAttributesOnTarget(targetAttr->GetAttributeComponent(), attributeMod);
	//	//}
	//}
}

float UGAAttributesBlueprintFunctionLibrary::ChangeAttributes(const FHitResult& Target, AActor* Instigator, TArray<FGAAttributeSpec> AttributesIn)
{
	float FinalValueMod = 0;
	//IIGAAttributes* instAttr = Cast<IIGAAttributes>(Instigator);
	//IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.Actor.Get());

	//if (!instAttr || !targetAttr)
	//	return FinalValueMod;

	//FGAAttributeModData AttMod;
	//FGAttributeContext AttrCtx;
	//AttrCtx.Target = targetAttr->GetAttributeComponent();
	//AttrCtx.Instigator = instAttr->GetAttributeComponent();
	//AttMod.AttributeContext = AttrCtx;
	//AttMod.AttributeModSpec = AttributesIn;
	//AttMod.HitLocation = Target.Location;
	////AttMod.AttributeContext.Instigator->ModifyAttributesOnTarget(AttMod.AttributeContext.Target.Get(), AttMod);
	//AttMod.ApplyMod();
	return FinalValueMod;
}

void UGAAttributesBlueprintFunctionLibrary::DamageAttribute(FGAAttribute AttributeIn, float ModValue, const FHitResult& TargetIn, const FGameplayTagContainer& TagsIn, APawn* Instigator)
{
	//IIGAAttributes* attributeIn = Cast<IIGAAttributes>(TargetIn.Actor.Get());
	//IIGAAttributes* attributeInst = Cast<IIGAAttributes>(Instigator);
	//if (!attributeIn || !attributeInst)
	//	return;

	//UGAAttributeComponent* attrComp = attributeInst->GetAttributeComponent();
	//if (!attrComp)
	//	return;

	//FGAAttributeModifier AttrMod;
	//AttrMod.HitTarget = TargetIn;
	//AttrMod.Instigator = Instigator;
	//AttrMod.Attribute = AttributeIn;
	//AttrMod.Value = ModValue;
	//AttrMod.Operation = EGAAttributeOp::Subtract;
	//AttrMod.Tags = TagsIn;

	//attrComp->ModifyAttributesOnTarget(attributeIn->GetAttributeComponent(), AttrMod);
}