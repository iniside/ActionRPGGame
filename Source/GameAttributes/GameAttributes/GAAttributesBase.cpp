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

UProperty* UGAAttributesBase::FindProperty(FGAAttribute AttributeIn)
{
	//if new attribute name is the same as last attribute name and pointer to last property
	//is not null, then we just return last pointer instead of performing new search.
	if ((AttributeIn.AttributeName == LastAttributeName) && LastAttributeProp)
		return LastAttributeProp;

	LastAttributeName = AttributeIn.AttributeName;
	LastAttributeProp = FindFieldChecked<UProperty>(this->GetClass(), LastAttributeName);
	return LastAttributeProp;
}

float UGAAttributesBase::GetFloatValue(FGAAttribute AttributeIn)
{
	if ((AttributeIn.AttributeName == LastAttributeName))
	{
		if (CachedFloatPropety)
		{
			const void* ValuePtr = CachedFloatPropety->ContainerPtrToValuePtr<void>(this);
			return CachedFloatPropety->GetFloatingPointPropertyValue(ValuePtr);
		}
	}
	LastAttributeName = AttributeIn.AttributeName;
	UNumericProperty* NumericProperty = CastChecked<UNumericProperty>(FindProperty(AttributeIn));
	CachedFloatPropety = NumericProperty;
	const void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(this);
	return NumericProperty->GetFloatingPointPropertyValue(ValuePtr);


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