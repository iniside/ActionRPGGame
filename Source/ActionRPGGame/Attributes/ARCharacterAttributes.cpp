// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "Attributes/GSAttributeComponent.h"
#include "GAAttributesStats.h"

#include "Net/UnrealNetwork.h"

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
	Health.BaseValue = 300;
}
void UARCharacterAttributes::InitializeAttributes()
{

	Super::InitializeAttributes();
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

FGAAttributeData UARCharacterAttributes::PreModifyAttribute(const FGAAttributeData& AttributeMod)
{
	FName attribute = AttributeMod.Attribute.AttributeName;
	ARCharacterAttributes_eventInternalPreModifyAttribute_Parms params;
	
	params.AttributeMod = AttributeMod;
	FString prefix = "PreAttribute_";
	prefix.Append(attribute.ToString());

	TWeakObjectPtr<UFunction> NativeFunc = GetClass()->FindFunctionByName(*prefix);

	if (NativeFunc.IsValid())
	{
		ProcessEvent(NativeFunc.Get(), &params);
		
		return params.ReturnValue;
	}
	return AttributeMod;
}
/*
	To much copy pasting:
	1. Hard to fix mistakes (if pasted formula was flawed.. we have to find and fix it everywhere).
	2. But it's pretty efficient, sinve we operate directly on data.
	3. Still, one point of entry would be better, especially for complex system like this, where
	there are doznes of possible damage types, and bonuses to them. Need way to make it
	completly Data Driven, where right formula will be determined on the fly.
*/
FGAAttributeData UARCharacterAttributes::PreAttribute_Damage(const FGAAttributeData& AttributeMod)
{
	//UARCharacterAttributes* InstiAttr = Cast<UARCharacterAttributes>(AttributeMod.Context.InstigatorComp->DefaultAttributes);
	FGAAttributeData returnData = AttributeMod;

	//returnData.Value = returnData.Value + InstiAttr->BonusDamage.GetAdditiveBonus();
	//returnData.Value = returnData.Value - InstiAttr->BonusDamage.GetSubtractBonus();

	//returnData.Value = returnData.Value + (returnData.Value * InstiAttr->BonusDamage.GetMultiplyBonus());
	//returnData.Value = returnData.Value - (returnData.Value * InstiAttr->BonusDamage.GetDivideBonus());

	////so additive bonus in defense would work like penalty and increas damage received ?
	//returnData.Value = returnData.Value + DamageDefense.GetAdditiveBonus();
	//returnData.Value = returnData.Value - DamageDefense.GetSubtractBonus();

	////the same goes for multiply bonus. Anything above zero will increase damage taken.
	//returnData.Value = returnData.Value + (returnData.Value * DamageDefense.GetMultiplyBonus());
	//returnData.Value = returnData.Value - (returnData.Value * DamageDefense.GetDivideBonus());

	return returnData;
}

FGAAttributeData UARCharacterAttributes::PreAttribute_FireDamage(const FGAAttributeData& AttributeMod)
{
	UARCharacterAttributes* InstiAttr = Cast<UARCharacterAttributes>(AttributeMod.Context.InstigatorComp->DefaultAttributes);
	FGAAttributeData returnData = AttributeMod;
	//float AddtiveBonus = InstiAttr->BonusDamage.GetAdditiveBonus() + InstiAttr->BonusFireDamage.GetAdditiveBonus();
	//float SubtractBonus = InstiAttr->BonusDamage.GetSubtractBonus() + InstiAttr->BonusFireDamage.GetSubtractBonus();
	//float MultiplyBonus = InstiAttr->BonusDamage.GetMultiplyBonus() + InstiAttr->BonusFireDamage.GetMultiplyBonus();
	//float DivideBonus = InstiAttr->BonusDamage.GetDivideBonus() + InstiAttr->BonusFireDamage.GetDivideBonus();
	//
	//returnData.Value = returnData.Value + AddtiveBonus;
	//returnData.Value = returnData.Value - SubtractBonus;

	//returnData.Value = returnData.Value + (returnData.Value * MultiplyBonus); 
	//returnData.Value = returnData.Value - (returnData.Value * DivideBonus);

	////so additive bonus in defense would work like penalty and increas damage received ?
	//returnData.Value = returnData.Value + DamageDefense.GetAdditiveBonus(); 
	//returnData.Value = returnData.Value - DamageDefense.GetSubtractBonus();

	////the same goes for multiply bonus. Anything above zero will increase damage taken.
	//returnData.Value = returnData.Value + (returnData.Value * DamageDefense.GetMultiplyBonus()); 
	//returnData.Value = returnData.Value - (returnData.Value * DamageDefense.GetDivideBonus());
	/*
		While the above make some theoretical sense, when you think about,
		It is not immiedietly obvious at first glance.
	*/
	return returnData;
}

float UARCharacterAttributes::PostModifyAttribute(const FGAEvalData& AttributeMod)
{
	SCOPE_CYCLE_COUNTER(STAT_PostModifyAttribute);

	FName attribute = AttributeMod.Attribute.AttributeName;
	ARCharacterAttributes_eventInternalPostModifyAttribute_Parms params;
	params.AttributeMod = AttributeMod;
	FString prefix = "PostAttribute_";
	prefix.Append(attribute.ToString());
	TWeakObjectPtr<UFunction> NativeFunc = PostModifyAttributeFunctions.FindRef(attribute);// GetClass()->FindFunctionByName(*prefix);

	if (NativeFunc.IsValid())
	{
		ProcessEvent(NativeFunc.Get(), &params);
		return params.ReturnValue;
	}
	return 0;
}


float UARCharacterAttributes::PostAttribute_Damage(const FGAEvalData& AttributeMod)
{
	Health.Subtract(Damage);
	float finalDamage = Damage;// Damage;
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
			OwningAttributeComp->GetOwner()->TakeDamage(finalDamage, goAway, nullptr, nullptr);
		}
		//handle death/
	}
	return finalDamage;
}
float UARCharacterAttributes::PostAttribute_FireDamage(const FGAEvalData& AttributeMod)
{
	Health.Subtract(FireDamage);
	float finalDamage = FireDamage;// FireDamage;
	FireDamage = 0;

	if (Health.GetCurrentValue() <= 0)
	{
		if (OwningAttributeComp)
		{
			//not best solution
			//but Damage system is so tighly ingrained into
			//engine, that we can just as well take advantage of build in events
			//for some stuff.
			FDamageEvent goAway;
			OwningAttributeComp->GetOwner()->TakeDamage(finalDamage, goAway, nullptr, nullptr);
		}
		//handle death/
	}
	return finalDamage;
}
float UARCharacterAttributes::PostAttribute_Heal(const FGAEvalData& AttributeMod)
{
	//Health.Add(Heal);
	Heal = 0;
	return Heal;
}
float UARCharacterAttributes::PostAttribute_LifeStealDamage(const FGAEvalData& AttributeMod)
{
	return 0;
}

float UARCharacterAttributes::PostAttribute_HealthBakPrecentageReduction(const FGAEvalData& AttributeMod)
{
	return 0;
}

float UARCharacterAttributes::PostAttribute_Magic(const FGAEvalData& AttributeMod)
{
	float finalValue = 0;
	//finalValue = Magic.GetCurrentValue();
	//finalValue = (finalValue - 10) / 2;
	//MagicMod.SetBaseValue(finalValue);
	return finalValue;
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


void UARCharacterAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UARCharacterAttributes, Health);
	DOREPLIFETIME(UARCharacterAttributes, Energy);
	DOREPLIFETIME(UARCharacterAttributes, Stamina);
}