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

void UGAAttributeMod::OnAttributeModifyOutCheck(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeOut)
{
	if (AttributeParam.Tags.MatchesAll(RequiredTags, true)) //might change it to false later.
	{
		OnAttributeModifyOut(AttributeParam, AttributeOut);
	}
}
void UGAAttributeMod::OnAttributeModifyInCheck(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeOut)
{
	if (AttributeParam.Tags.MatchesAll(RequiredTags, true)) //might change it to false later.
	{
		OnAttributeModifyIn(AttributeParam, AttributeOut);
	}
}

void UGAAttributeMod::OnAttributeModifyOut_Implementation(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeOut)
{
	AttributeOut = AttributeParam;
}


void UGAAttributeMod::OnAttributeModifyIn_Implementation(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeIn)
{
	AttributeIn = AttributeParam;
}