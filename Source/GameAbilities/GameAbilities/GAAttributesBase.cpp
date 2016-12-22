// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "GAGlobalTypes.h"
#include "GAAttributesBase.h"

UGAAttributesBase::UGAAttributesBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bNetAddressable = false;
	LastAttributeProp = nullptr;
	CachedFloatPropety = nullptr;
}
UGAAttributesBase::~UGAAttributesBase()
{
	LastAttributeProp = nullptr; //make sure we clear this pointer.
	CachedFloatPropety = nullptr;
}

void UGAAttributesBase::InitializeAttributes()
{
	for (TFieldIterator<UStructProperty> StrIt(GetClass(), EFieldIteratorFlags::IncludeSuper); StrIt; ++StrIt)
	{
		FGAAttributeBase* attr = StrIt->ContainerPtrToValuePtr<FGAAttributeBase>(this);
		if (attr)
		{
			attr->InitializeAttribute();
		}
	}
	/*
		Bind Delegates to map > For each attribute, so we don't store them inside attribute
		but in this class.
	*/
	/*for (TFieldIterator<UProperty> PropIt(GetClass(), EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
	{
		int32 FoundIndex = -1;
		FoundIndex = PropIt->GetName().Find("PostAttribute");
		FGAAttributeBase* attrPtr = GetAttribute(FGAAttribute(PropIt->GetFName()));
		if (attrPtr)
		{
			return attrPtr->InitializeAttribute();
		}
	}*/
	BP_InitializeAttributes();
}



UProperty* UGAAttributesBase::FindProperty(const FGAAttribute& AttributeIn)
{
	//if new attribute name is the same as last attribute name and pointer to last property
	//is not null, then we just return last pointer instead of performing new search.
	if ((AttributeIn.AttributeName == LastAttributeName) && LastAttributeProp)
		return LastAttributeProp;

	LastAttributeName = AttributeIn.AttributeName;
	LastAttributeProp = FindFieldChecked<UProperty>(this->GetClass(), LastAttributeName);
	return LastAttributeProp;
	return nullptr;
}
UStructProperty* UGAAttributesBase::GetStructAttribute(const FGAAttribute& Name)
{
	return FindField<UStructProperty>(this->GetClass(), Name.AttributeName);
}
FGAAttributeBase* UGAAttributesBase::GetAttribute(const FGAAttribute& Name)
{
	UStructProperty* tempStruct = FindField<UStructProperty>(this->GetClass(), Name.AttributeName);

	FGAAttributeBase* attr = nullptr;
	if (tempStruct)
	{
		attr = tempStruct->ContainerPtrToValuePtr<FGAAttributeBase>(this);
		return attr;
	} 
	return attr;
}
void UGAAttributesBase::SetAttribute(const FGAAttribute& NameIn, UObject* NewVal)
{
	//UStructProperty* tempStruct = FindField<UStructProperty>(this->GetClass(), NameIn.AttributeName);
}
void UGAAttributesBase::SetAttributeAdditiveBonus(const FGAAttribute& NameIn, float NewValue)
{
	UStructProperty* tempStruct = FindField<UStructProperty>(this->GetClass(), NameIn.AttributeName);
	UScriptStruct* scriptStruct = tempStruct->Struct;

	uint8* StructData = tempStruct->ContainerPtrToValuePtr<uint8>(this);
	
	//omg figured it out!
	//for (TFieldIterator<UProperty> It(scriptStruct); It; ++It)
	//{
	//	if (UProperty* Prop = *It)
	//	{
	//		if (Prop->GetFName() == "AdditiveBonus")
	//		{
	//			if (Prop->IsA(UFloatProperty::StaticClass()))
	//			{
	//				float testValue = NewValue;
	//				Cast<UFloatProperty>(Prop)->SetPropertyValue_InContainer(StructData, testValue);
	//				break;
	//			}
	//		}
	//	}
	//}
}

float UGAAttributesBase::GetFinalAttributeValue(const FGAAttribute& Name)
{
	FGAAttributeBase* attrPtr = GetAttribute(Name);
	if (attrPtr)
	{
		return attrPtr->GetFinalValue();
	}
	return 0;
}
float UGAAttributesBase::GetCurrentAttributeValue(const FGAAttribute& Name)
{
	FGAAttributeBase* attrPtr = GetAttribute(Name);
	if (attrPtr)
	{
		return attrPtr->GetCurrentValue();
	}
	return 0;
}
float UGAAttributesBase::GetFloatValue(const FGAAttribute& AttributeIn)
{
	if ((AttributeIn.AttributeName == LastAttributeName))
	{
		if (CachedFloatPropety)
		{
			const void* ValuePtr = CachedFloatPropety->ContainerPtrToValuePtr<void>(this);
			return CachedFloatPropety->GetFloatingPointPropertyValue(ValuePtr);
		}
	}
	//LastAttributeName = AttributeIn.AttributeName;
	UNumericProperty* NumericProperty = CastChecked<UNumericProperty>(FindProperty(AttributeIn));
	CachedFloatPropety = NumericProperty;
	const void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(this);
	return NumericProperty->GetFloatingPointPropertyValue(ValuePtr);

	return 0;
}

float UGAAttributesBase::SetFloatValue(const FGAAttribute& AttributeIn, float ValueIn)
{
	if ((AttributeIn.AttributeName == LastAttributeName))
	{
		if (CachedFloatPropety)
		{
			void* ValuePtr = CachedFloatPropety->ContainerPtrToValuePtr<void>(this);
			CachedFloatPropety->SetFloatingPointPropertyValue(ValuePtr, ValueIn);
			return CachedFloatPropety->GetFloatingPointPropertyValue(ValuePtr);
		}
	}
	//LastAttributeName = AttributeIn.AttributeName;
	UNumericProperty* NumericProperty = CastChecked<UNumericProperty>(FindProperty(AttributeIn));
	CachedFloatPropety = NumericProperty;
	void* ValuePtr = CachedFloatPropety->ContainerPtrToValuePtr<void>(this);
	NumericProperty->SetFloatingPointPropertyValue(ValuePtr, ValueIn);
	return CachedFloatPropety->GetFloatingPointPropertyValue(ValuePtr);
	return 0;
}

float UGAAttributesBase::AttributeOperation(const FGAAttribute& AttributeIn, float ValueIn, EGAAttributeMod Operation)
{
	switch (Operation)
	{
	case EGAAttributeMod::Add:
		return AddAttributeFloat(GetFloatValue(AttributeIn), ValueIn); //don't want to set.
	case EGAAttributeMod::Subtract:
		return SubtractAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeMod::Multiply:
		return MultiplyAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeMod::Divide:
		return DivideAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeMod::Set:
		return SetFloatValue(AttributeIn, ValueIn);
	default:
		return 0;
	}
	return 0;
}

float UGAAttributesBase::AddAttributeFloat(float ValueA, float ValueB)
{
	return ValueA + ValueB;
}
float UGAAttributesBase::SubtractAttributeFloat(float ValueA, float ValueB)
{
	return ValueA - ValueB;
}
float UGAAttributesBase::MultiplyAttributeFloat(float ValueA, float ValueB)
{
	return ValueA * ValueB;
}
float UGAAttributesBase::DivideAttributeFloat(float ValueA, float ValueB)
{
	return ValueA / ValueB;
}

bool UGAAttributesBase::IsNameStableForNetworking() const
{
	/**
	* IsNameStableForNetworking means an attribute set can be referred to its path name (relative to owning AActor*) over the network
	*
	* Attribute sets are net addressable if:
	*	-They are Default Subobjects (created in C++ constructor)
	*	-They were loaded directly from a package (placed in map actors)
	*	-They were explicitly set to bNetAddressable
	*/

	return bNetAddressable;// || Super::IsNameStableForNetworking();
}


void UGAAttributesBase::SetNetAddressable()
{
	bNetAddressable = true;
}
void UGAAttributesBase::ModifyAttribute(const FGAEffect& EffectIn)
{

}

float UGAAttributesBase::ModifyAttribute(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn)
{
	FGAAttributeBase* attr = nullptr;

	attr = GetAttribute(ModIn.Attribute);
	if (attr)
	{
		return attr->Modify(ModIn, HandleIn);
	}
	return -1;
}