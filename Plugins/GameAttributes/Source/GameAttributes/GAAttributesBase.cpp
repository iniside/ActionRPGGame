// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

UGAAttributesBase::UGAAttributesBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bNetAddressable = false;
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