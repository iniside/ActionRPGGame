// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "Attributes/GSAttributeComponent.h"
#include "ARCharacterAttributes.h"

UARCharacterAttributes::UARCharacterAttributes(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UARCharacterAttributes::PostInterpChange(UProperty* PropertyThatChanged)
{
	FName propName = PropertyThatChanged->GetFName();
	if (propName == "Magic")
	{
		MagicMod = (Magic - 10) / 2;
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
}

bool UARCharacterAttributes::PostModifyAttribute_Implementation(const FGAEvalData& AttributeMod)
{
	UProperty* ChangeAttrPtr = FindFieldChecked<UProperty>(this->GetClass(), AttributeMod.Attribute.AttributeName);
	if (!ChangeAttrPtr)
		return false;

	if (ChangeAttrPtr->GetFName() == "FireDamage")
	{
		//do something with fire damage
		Health -= FireDamage;
		FireDamage = 0;
		return true;
	}
	if (ChangeAttrPtr->GetFName() == "LifeStealDamage")
	{
		//do something with fire damage
		Health -= LifeStealDamage;
		//we need to add LifeStealDamage to instigator. In some reasonable way that is.
		FGAAttribute InstigatorHealth;
		InstigatorHealth.AttributeName = TEXT("Health");
		float InstHealthFloat = AttributeMod.AttributeContext.Instigator->DefaultAttributes->GetFloatValue(InstigatorHealth);
		InstHealthFloat += LifeStealDamage;
		FGAEvalData set = AttributeMod;
		set.Attribute = InstigatorHealth;
		set.ModValue = InstHealthFloat;
		AttributeMod.AttributeContext.Instigator->DefaultAttributes->UpdateAttributes(set, InstHealthFloat);
		//and this is not reasonable way.


		LifeStealDamage = 0;
		return true;
	}

	if (ChangeAttrPtr->GetFName() == "Magic")
	{
		MagicMod = (Magic - 10) / 2;
		return true;
	}

	return false;
}