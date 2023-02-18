// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"

#include "../AFAbilityInterface.h"
#include "GAAttributesBase.h"
#include "../AFAbilityComponent.h"
#include "GABlueprintLibrary.h"
#include "GAAttributesBlueprintFunctionLibrary.h"



UGAAttributesBlueprintFunctionLibrary::UGAAttributesBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UGAAttributesBlueprintFunctionLibrary::EqualAttribute(const FGAAttribute& Compare, FGAAttribute Against)
{
	return Compare == Against;
}
FName UGAAttributesBlueprintFunctionLibrary::GetAttribute(FGAAttribute AttributeIn)
{
	return AttributeIn.AttributeName;
}
float UGAAttributesBlueprintFunctionLibrary::GetFinalAttributeValue(AActor* Target, FGAAttribute Name)
{
	IAFAbilityInterface* attributeInt = Cast<IAFAbilityInterface>(Target);
	if (!attributeInt)
		return 0;
	if (!attributeInt->GetAttributes())
		return 0;

	return attributeInt->GetAttributes()->GetFinalAttributeValue(Name);
}
float UGAAttributesBlueprintFunctionLibrary::GetCurrentAttributeValue(AActor* Target, FGAAttribute Name)
{
	IAFAbilityInterface* attributeInt = Cast<IAFAbilityInterface>(Target);
	if (!attributeInt)
		return 0;
	return attributeInt->GetAttributes()->GetCurrentAttributeValue(Name);
}
float UGAAttributesBlueprintFunctionLibrary::GetAttributeFloat(AActor* Target, FGAAttribute AttributeIn)
{
	IAFAbilityInterface* attributeInt = Cast<IAFAbilityInterface>(Target);
	if (!attributeInt)
		return 0;

	return  attributeInt->GetAttributes()->GetFloatValue(AttributeIn);
}

void UGAAttributesBlueprintFunctionLibrary::ExchangeAttributesValues(
	APawn* Instigator
	, UObject* Causer
	, FAFPropertytHandle From
	, FGAEffectHandle FromHandle
	, UObject* FromTarget
	, FAFPropertytHandle To
	, FGAEffectHandle ToHandle
	, UObject* ToTarget)
{
	IAFAbilityInterface* FromInterface = Cast<IAFAbilityInterface>(FromTarget);
	IAFAbilityInterface* ToInterface = Cast<IAFAbilityInterface>(ToTarget);

	if (!FromInterface || !ToInterface)
		return;

	FAFFunctionModifier ModF;
	UGABlueprintLibrary::ApplyGameEffectToObject(From, FromTarget, Instigator, Causer, ModF);
	UGABlueprintLibrary::ApplyGameEffectToObject(To, ToTarget, Instigator, Causer, ModF);
}