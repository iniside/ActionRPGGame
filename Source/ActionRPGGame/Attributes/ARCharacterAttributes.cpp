// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "Attributes/GSAttributeComponent.h"
#include "GAAttributesStats.h"
#include "ARCharacterAttributes.h"

DEFINE_STAT(STAT_PostModifyAttribute);
DEFINE_STAT(STAT_PostEffectApplied);
DEFINE_STAT(STAT_OutgoingAttribute);
DEFINE_STAT(STAT_IncomingAttribute);
UARCharacterAttributes::UARCharacterAttributes(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Damage = 0;
	FireDamage = 0;
}
void UARCharacterAttributes::InitializeAttributes()
{

	//construct array of all attributes, and assign indexes for them.
	//it should be done only once upon initialization!
	//actually we could filter out attributes per group
	//we are going to cache all functions.
	//that's realll bad, for now
	//because you have to name your functions in very specific way.
	PostModifyAttributeFunctions.Empty();
	IncomingModifyAttributeFunctions.Empty();
	OutgoingModifyAttributeFunctions.Empty();
	for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	{
		int32 FoundIndex = -1;
		FoundIndex = FuncIt->GetName().Find("PostAttribute");
		if (FoundIndex > -1)
		{
			FString postName = FuncIt->GetName();
			postName = postName.RightChop(14);
			FName keyIn = *postName;
			PostModifyAttributeFunctions.Add(keyIn, *FuncIt);
		}
	}
	//for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	//{
	//	if (FuncIt->GetMetaData("Category") == "Outgoing")
	//	{
	//		FString name = FuncIt->GetName();
	//		name = name.RightChop(9);
	//		//attempt to add function like Condition.Hex, which might happen to exist in blueprint.
	//		FName keyIn = *name;
	//		OutgoingModifyAttributeFunctions.Add(keyIn, *FuncIt);
	//		//then we will look for functions in C++.
	//		name = name.Replace(TEXT("_"), TEXT("."));
	//		keyIn = *name;
	//		OutgoingModifyAttributeFunctions.Add(keyIn, *FuncIt);
	//	}
	//}
	//for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	//{
	//	if (FuncIt->GetMetaData("Category") == "Incoming")
	//	{
	//		FString name = FuncIt->GetName();
	//		name = name.RightChop(9);
	//		name = name.Replace(TEXT("_"), TEXT("."));
	//		FName keyIn = *name;
	//		IncomingModifyAttributeFunctions.Add(keyIn, *FuncIt);
	//	}
	//}

	for (TFieldIterator<UStructProperty> StrIt(GetClass(), EFieldIteratorFlags::IncludeSuper); StrIt; ++StrIt)
	{
		FGAAttributeBase* attr = StrIt->ContainerPtrToValuePtr<FGAAttributeBase>(this);
		if (attr)
		{
			attr->InitializeAttribute();
		}
	}
}

void UARCharacterAttributes::PostEffectApplied()
{
	SCOPE_CYCLE_COUNTER(STAT_PostEffectApplied);
	//FName Tag = SpecIn.MyTag.GetTagName();
	//FString nam = "OnEffectApplied_";
	//Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	//nam = nam.Append(Tag.ToString());
	//UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
}
void UARCharacterAttributes::PostEffectRemoved()
{
	//FName Tag = SpecIn.MyTag.GetTagName();
	//FString nam = "OnEffectApplied_";
	//Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	//nam = nam.Append(Tag.ToString());
	//UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
}


void UARCharacterAttributes::PostModifyAttribute(const FGAEvalData& AttributeMod)
{
	SCOPE_CYCLE_COUNTER(STAT_PostModifyAttribute);

	FName attribute = AttributeMod.Attribute.AttributeName;
	ARCharacterAttributes_eventInternalPostModifyAttribute_Parms params;
	params.AttributeMod = AttributeMod;
	FString prefix = "PostAttribute_";
	prefix.Append(attribute.ToString());

	TWeakObjectPtr<UFunction> NativeFunc = PostModifyAttributeFunctions.FindRef(attribute);// GetClass()->FindFunctionByName(*prefix);
	//if (!NativeFunc)
	//	return Callback;

	if (NativeFunc.IsValid())
	{
		ProcessEvent(NativeFunc.Get(), &params);
	}
}


void UARCharacterAttributes::PostAttribute_Damage(const FGAEvalData& AttributeMod)
{
	Health.Subtract(Damage);
	Damage = 0;

	if (Health.GetCurrentValue() <= 0)
	{
		if (OwningAttributeComp)
		{
			//not best solution
			//but Damage system is so tighly ingrained into
			//engine, that we can just as well take advantage of build in events
			//for some stuff.
			FDamageEvent goAway;
			OwningAttributeComp->GetOwner()->TakeDamage(Damage, goAway, nullptr, nullptr);
		}
		//handle death/
	}

}
void UARCharacterAttributes::PostAttribute_Heal(const FGAEvalData& AttributeMod)
{
	Health.Add(Heal);
	Heal = 0;
}
void UARCharacterAttributes::PostAttribute_LifeStealDamage(const FGAEvalData& AttributeMod)
{

}

void UARCharacterAttributes::PostAttribute_HealthBakPrecentageReduction(const FGAEvalData& AttributeMod)
{

}

void UARCharacterAttributes::PostAttribute_Magic(const FGAEvalData& AttributeMod)
{
	float finalValue = 0;
	finalValue = Magic.GetCurrentValue();
	finalValue = (finalValue - 10) / 2;
	MagicMod.SetBaseValue(finalValue);
}


void UARCharacterAttributes::CalculateOutgoingAttributeMods()
{
	SCOPE_CYCLE_COUNTER(STAT_OutgoingAttribute);
}
void UARCharacterAttributes::CalculateIncomingAttributeMods()
{
	SCOPE_CYCLE_COUNTER(STAT_IncomingAttribute);
	//FName Tag = AttributeModIn.AttributeTag.GetTagName();
	//FString nam = "Incoming_";
	//ARCharacterAttributes_eventInternalIncomingAttributeMod_Parms params;
	//params.AttributeModIn = AttributeModIn;
	//Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	//nam = nam.Append(Tag.ToString());

	//UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
	//if (ModifyAttrFunc)
	//{
	//	ProcessEvent(ModifyAttrFunc, &params);
	//}
}
