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
	UPROPERTY()
		FGAAttribute Attribute;
	
	void OnPreAttributeModify(class UAFAbilityComponent* InComp, const FGAAttribute& Attribute, float InValue);
	void OnPostAttributeModify(class UAFAbilityComponent* InComp, const FGAAttribute& Attribute, float InValue);

	virtual void PreAttributeModify(const FGAEffectContext& InContext, float PreValue) {};
	virtual void PostAttributeModify(const FGAEffectContext& InContext, float PreValue, float PostValue) {};

	virtual float CalculateBonusValueByTags(const FGAIndividualMods& Mods) { return 0; }
	virtual float CalculateCurentValue() { return 0; }
	virtual bool CanModifyAttribute() { return true; }
	
};
