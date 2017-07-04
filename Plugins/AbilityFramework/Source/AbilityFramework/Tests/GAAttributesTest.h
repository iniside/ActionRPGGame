// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Attributes/GAAttributesBase.h"
#include "GAAttributesTest.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAttributesTest : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Energy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Magic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Inteligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicDamageBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicDamageMultiplayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicDefensePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicDefenseFlat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicalBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase PhysicalBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicResistance;
public:
	virtual void InitializeAttributes(UAFAbilityComponent* InOwningAttributeComp) override;
};
