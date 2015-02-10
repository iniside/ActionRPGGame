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
	int32 Counter = 0;
	for (TFieldIterator<UProperty> PropIt(GetClass(), EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
	{
		FGAAttributeNode node;
		node.AttributeIndex = Counter;
		node.AttributeName = PropIt->GetFName();
		AttributeNodes.Add(node);
		Counter++;
	}
	int32 nodesNum = AttributeNodes.Num();
	for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	{
		if (FuncIt->GetMetaData("Category") == "PostAttribute")
		{
			FString name = FuncIt->GetName();
			name = name.RightChop(14);
			FName keyIn = *name;
			//it should ensure that related functions
			//are added in the same order as there are attributes.
			for (int32 i = 0; i < nodesNum; i++)
			{
				if (AttributeNodes[i].AttributeName == keyIn)
				{
					AttributeNodes[i].AttributeFunction = *FuncIt;
				}
			}
		}
	}
	float test = 0;
	//we are going to cache all functions.
	//that's realll bad, for now
	//because you have to name your functions in very specific way.
	PostModifyAttributeFunctions.Empty();
	IncomingModifyAttributeFunctions.Empty();
	OutgoingModifyAttributeFunctions.Empty();
	for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	{
		if (FuncIt->GetMetaData("Category") == "PostAttribute")
		{
			FString name = FuncIt->GetName();
			name = name.RightChop(14);
			FName keyIn = *name;
			PostModifyAttributeFunctions.Add(keyIn, *FuncIt);
		}
	}
	for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	{
		if (FuncIt->GetMetaData("Category") == "Outgoing")
		{
			FString name = FuncIt->GetName();
			name = name.RightChop(9);
			//attempt to add function like Condition.Hex, which might happen to exist in blueprint.
			FName keyIn = *name;
			OutgoingModifyAttributeFunctions.Add(keyIn, *FuncIt);
			//then we will look for functions in C++.
			name = name.Replace(TEXT("_"), TEXT("."));
			keyIn = *name;
			OutgoingModifyAttributeFunctions.Add(keyIn, *FuncIt);
		}
	}
	for (TFieldIterator<UFunction> FuncIt(GetClass(), EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
	{
		if (FuncIt->GetMetaData("Category") == "Incoming")
		{
			FString name = FuncIt->GetName();
			name = name.RightChop(9);
			name = name.Replace(TEXT("_"), TEXT("."));
			FName keyIn = *name;
			IncomingModifyAttributeFunctions.Add(keyIn, *FuncIt);
		}
	}

	for (TFieldIterator<UStructProperty> StrIt(GetClass(), EFieldIteratorFlags::IncludeSuper); StrIt; ++StrIt)
	{
		FGAAttributeBase* attr = StrIt->ContainerPtrToValuePtr<FGAAttributeBase>(this);
		if (attr)
		{
			attr->InitializeAttribute();
		}
	}
}
void UARCharacterAttributes::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName propName = PropertyChangedEvent.Property->GetFName();
	if (propName == "Magic")
	{
		MagicMod = (Magic - 10) / 2;
	}
	if (propName == "HealthBak")
	{
		MaxHealthBak = HealthBak;
	}
}

void UARCharacterAttributes::PostEffectApplied(FGAEffectBase& SpecIn)
{
	SCOPE_CYCLE_COUNTER(STAT_PostEffectApplied);
	FName Tag = SpecIn.MyTag.GetTagName();
	FString nam = "OnEffectApplied_";
	Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	nam = nam.Append(Tag.ToString());
	UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
}
void UARCharacterAttributes::PostEffectRemoved(FGAEffectBase& SpecIn)
{
	FName Tag = SpecIn.MyTag.GetTagName();
	FString nam = "OnEffectApplied_";
	Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	nam = nam.Append(Tag.ToString());
	UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
}

void UARCharacterAttributes::OnEffectApplied_Hex(const FGAEffectBase& AttributeMod)
{
	HexesCount += 1;
}
void UARCharacterAttributes::OnEffectRemoved_Hex(const FGAEffectBase& AttributeMod)
{
	HexesCount -= 1;
}

void UARCharacterAttributes::OnEffectApplied_Condition_Weakness(const FGAEffectBase& AttributeMod)
{

}
void UARCharacterAttributes::OnEffectRemoved_Condition_Weakness(const FGAEffectBase& AttributeMod)
{

}

FGAAttributeDataCallback UARCharacterAttributes::PostModifyAttribute(const FGAEvalData& AttributeMod)
{
	SCOPE_CYCLE_COUNTER(STAT_PostModifyAttribute);
	FGAAttributeDataCallback Callback;

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
		return params.ReturnValue;
	}
	
	return Callback;
}

//FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_HealthBak(const FGAEvalData& AttributeMod)
//{
//	//HealthBak -= Damage;
//	FGAAttributeDataCallback Callback;
//	Callback.AttributeContext = AttributeMod.AttributeContext;
//	Callback.Attribute = FGAAttribute(TEXT("Damage"));
//	Callback.AttributeValue = Damage;
//	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
//	Callback.ModifiedAttributeValue = HealthBak;
//	//and we update attribute that has been modified on us. In this case we update HealthBak.
//	FGAEvalData EvalData;
//	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
//	EvalData.ModValue = HealthBak;
//	EvalData.AttributeContext = AttributeMod.AttributeContext;
//	PostModifyAttribute(EvalData);
//
//	//UpdateAttributeSelf(Callback);
//	Damage = 0;
//
//	return Callback;
//}

FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Damage(const FGAEvalData& AttributeMod)
{
	Health.Subtract(Damage);
	//Health.CalculateBonus();
	FGAAttributeDataCallback Callback;
	//Callback.AttributeContext = AttributeMod.AttributeContext;
	//Callback.Attribute = FGAAttribute(TEXT("Damage"));
	//Callback.AttributeValue = Damage;
	//Callback.ModifiedAttribute = FGAAttribute(TEXT("Health"));
	//Callback.ModifiedAttributeValue = HealthBak;
	////and we update attribute that has been modified on us. In this case we update HealthBak.
	//FGAEvalData EvalData;
	//EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	//EvalData.ModValue = HealthBak;
	//EvalData.AttributeContext = AttributeMod.AttributeContext;
	//PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;
	
	return Callback;
}

FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_LifeStealDamage(const FGAEvalData& AttributeMod)
{
	FGAAttributeDataCallback Callback;
	//do something with fire damage
	HealthBak -= LifeStealDamage;
	//we need to add LifeStealDamage to instigator. In some reasonable way that is.
	FGAAttribute InstigatorHealthBak;
	InstigatorHealthBak.AttributeName = TEXT("HealthBak");
	float InstHealthBakFloat = AttributeMod.AttributeContext.InstigatorComp->DefaultAttributes->GetFloatValue(InstigatorHealthBak);
	InstHealthBakFloat += LifeStealDamage;
	FGAEvalData set = AttributeMod;
	set.Attribute = InstigatorHealthBak;
	set.ModValue = InstHealthBakFloat;

	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("LifeStealDamage"));
	Callback.AttributeValue = LifeStealDamage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;

	AttributeMod.AttributeContext.InstigatorComp->DefaultAttributes->UpdateAttributes(set, InstHealthBakFloat);
	//and this is not reasonable way.


	LifeStealDamage = 0;
	
	return Callback;
}

FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_HealthBakPrecentageReduction(const FGAEvalData& AttributeMod)
{
	FGAAttributeDataCallback Callback;
	float HealthBakReduction = MaxHealthBak * HealthBakPrecentageReduction;

	HealthBak -= HealthBakReduction;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("HealthBakPrecentageReduction"));
	Callback.AttributeValue = HealthBakPrecentageReduction;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBakReduction;

	HealthBakPrecentageReduction = 0;
	
	return Callback;
}

FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Magic(const FGAEvalData& AttributeMod)
{
	FGAAttributeDataCallback Callback;
	MagicMod = (Magic - 10) / 2;

	return Callback;
}


FGAAttributeSpec UARCharacterAttributes::CalculateOutgoingAttributeMods(const FGAAttributeSpec& AttributeModIn)
{
	SCOPE_CYCLE_COUNTER(STAT_OutgoingAttribute);
	if (AttributeModIn.AttributeTag.GetTagName() == "Damage.Fire")
	{
		return Outgoing_Damage_Fire(AttributeModIn);
	}
	return AttributeModIn;
}
FGAAttributeSpec UARCharacterAttributes::CalculateIncomingAttributeMods(const FGAAttributeSpec& AttributeModIn)
{
	SCOPE_CYCLE_COUNTER(STAT_IncomingAttribute);
	FName Tag = AttributeModIn.AttributeTag.GetTagName();
	FString nam = "Incoming_";
	ARCharacterAttributes_eventInternalIncomingAttributeMod_Parms params;
	params.AttributeModIn = AttributeModIn;
	Tag = *Tag.ToString().Replace(TEXT("."), TEXT("_"));
	nam = nam.Append(Tag.ToString());

	UFunction* ModifyAttrFunc = GetClass()->FindFunctionByName(*nam);
	if (ModifyAttrFunc)
	{
		ProcessEvent(ModifyAttrFunc, &params);
		return params.ReturnValue;
	}
	return AttributeModIn;
}


FGAAttributeSpec UARCharacterAttributes::Incoming_Damage_Fire(const FGAAttributeSpec& AttributeModIn)
{
	FGAAttributeSpec spec = AttributeModIn;

	return spec;
}

FGAAttributeSpec UARCharacterAttributes::Outgoing_Damage_Fire(const FGAAttributeSpec& AttributeModIn)
{
	FGAAttributeSpec spec = AttributeModIn;
	if (BonusDamage > BonusFireDamage)
	{
		float magnitude = spec.GetCalculatedMagnitude();
		magnitude = magnitude + magnitude * BonusDamage;
		magnitude = magnitude - magnitude * OutgoingDamageReduction;
		spec.SetCalculatedMagnitude(magnitude);
	}
	else
	{
		float magnitude = spec.GetCalculatedMagnitude();
		magnitude = magnitude + magnitude * BonusFireDamage;
		magnitude = magnitude - magnitude * OutgoingDamageReduction;
		spec.SetCalculatedMagnitude(magnitude);
	}
	return spec;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Strenght(const FGAEvalData& AttributeMod)
{
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}

FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Endurance(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Intelligence(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_WillPower(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Wisdom(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_ConditionDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_ConditionFireDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_ConditionBleedDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_ConditionPoisonDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_BonusDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_BonusPhysicalDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_BonusMagicalDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_FireDamageDefense(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_SpellCastingSpeed(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_AttackSpeed(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_SpellCostMultiplier(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_PhysicalAttackCostMultiplier(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_EnergyStealDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_StaminaStealDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_RunningMovementSpeed(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_WalkingMovementSpeed(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_PhysicalDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_MagicalDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_FireDamage(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Hex(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Somg(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Ehehehhee(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_AnotherAttribute(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Zomg(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Dunno(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Moar(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Roflolamo(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_EvenMoreFunction(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_Notsogood(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_postpooasd(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_uahskjd(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_kjashkd(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}


FGAAttributeDataCallback UARCharacterAttributes::PostAttribute_uayskahsd(const FGAEvalData& AttributeMod){
	HealthBak -= Damage;
	FGAAttributeDataCallback Callback;
	Callback.AttributeContext = AttributeMod.AttributeContext;
	Callback.Attribute = FGAAttribute(TEXT("Damage"));
	Callback.AttributeValue = Damage;
	Callback.ModifiedAttribute = FGAAttribute(TEXT("HealthBak"));
	Callback.ModifiedAttributeValue = HealthBak;
	//and we update attribute that has been modified on us. In this case we update HealthBak.
	FGAEvalData EvalData;
	EvalData.Attribute = FGAAttribute(TEXT("HealthBak"));
	EvalData.ModValue = HealthBak;
	EvalData.AttributeContext = AttributeMod.AttributeContext;
	PostModifyAttribute(EvalData);

	//UpdateAttributeSelf(Callback);
	Damage = 0;

	return Callback;
}
