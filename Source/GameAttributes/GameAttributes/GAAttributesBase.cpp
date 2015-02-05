// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
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
	BP_InitializeAttributes();
}

void UGAAttributesBase::UpdateAttributes(const FGAEvalData& AttributeIn, float newValue)
{
	SetFloatValue(AttributeIn.Attribute, newValue);
	PostModifyAttribute(AttributeIn);
}
bool UGAAttributesBase::PostModifyAttribute_Implementation(const FGAEvalData& AttributeMod)
{
	return false;
}
bool UGAAttributesBase::CalculateOutgoingAttributeMods_Implementation(const FGAAttributeSpec& AttributeModIn, FGAAttributeSpec& AttributeModOut)
{
	AttributeModOut = AttributeModIn;
	return false;
}
bool UGAAttributesBase::CalculateIncomingAttributeMods_Implementation(const FGAAttributeSpec& AttributeModIn, FGAAttributeSpec& AttributeModOut)
{
	AttributeModOut = AttributeModIn;
	return false;
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
}

float UGAAttributesBase::AttributeOperation(const FGAAttribute& AttributeIn, float ValueIn, EGAAttributeOp Operation)
{
	switch (Operation)
	{
	case EGAAttributeOp::Add:
		return AddAttributeFloat(GetFloatValue(AttributeIn), ValueIn); //don't want to set.
	case EGAAttributeOp::Subtract:
		return SubtractAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeOp::Multiply:
		return MultiplyAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeOp::Divide:
		return DivideAttributeFloat(GetFloatValue(AttributeIn), ValueIn);
	case EGAAttributeOp::Set:
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

	return bNetAddressable || Super::IsNameStableForNetworking();
}


void UGAAttributesBase::SetNetAddressable()
{
	bNetAddressable = true;
}