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
		class UAFAbilityComponent* AbilityComponent;
	UPROPERTY()
		FGAAttribute Attribute;
	void Initialize(UAFAbilityComponent* InAbilityComponent, const FName& InAttributeName);
	
	void OnPreAttributeModify(float InValue);
	void OnPostAttributeModify(float InValue);

	virtual void PreAttributeModify() {};
	virtual void PostAttributeModify() {};
	virtual float CalculateBonusValueByTags(const FGAIndividualMods& Mods) { return 0; }
	virtual float CalculateCurentValue() { return 0; }
	virtual bool CanModifyAttribute() { return true; }
	
};
