// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "GAAttributeExtension.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAttributeExtension : public UObject
{
	GENERATED_BODY()
public:
	virtual void PreAttributeModify() {};
	virtual void PostAttributeModify() {};
	virtual void OnPreAttributeModified() {};
	virtual void OnPostAttributeModified() {};
	virtual float CalculateBonusValueByTags(const FGAIndividualMods& Mods) { return 0; }
	virtual float CalculateCurentValue() { return 0; }
	virtual bool CanModifyAttribute() { return true; }
	
};
