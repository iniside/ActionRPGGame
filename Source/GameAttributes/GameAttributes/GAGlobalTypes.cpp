#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAGlobalTypes.h"

void FGAAttributeModData::ApplyMod()
{
	if (!AttributeContext.Instigator.IsValid())
		return;

	AttributeContext.Instigator->ModifyAttributesOnTarget(AttributeContext.Target.Get(), *this);
}