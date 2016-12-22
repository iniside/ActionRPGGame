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

void UARCharacterAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UARCharacterAttributes, Health);
	DOREPLIFETIME(UARCharacterAttributes, Energy);
	DOREPLIFETIME(UARCharacterAttributes, Stamina);
}