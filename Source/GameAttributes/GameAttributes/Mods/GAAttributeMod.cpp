// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GAAttributeMod.h"

UGAAttributeMod::UGAAttributeMod(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGAAttributeMod::BindDelegates(FGAOnAttributeOutgoing& Outgoing, FGAOnAttributeIncoming& Incoming)
{
	Outgoing.AddUObject(this, &UGAAttributeMod::OnAttributeModifyOut);
	Incoming.AddUObject(this, &UGAAttributeMod::OnAttributeModifyIn);
}