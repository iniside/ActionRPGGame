// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributesBase.h"
#include "AFDCharacterAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORKDEBUGGER_API UAFDCharacterAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase Health;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase Energy;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase Stamina;

	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase Armor;
	
};
