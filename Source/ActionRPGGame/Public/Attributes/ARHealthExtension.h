// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributeExtension.h"
#include "ARHealthExtension.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARHealthExtension : public UGAAttributeExtension
{
	GENERATED_BODY()
	
		virtual void PreAttributeModify(const FGAEffectContext& InContext, float PreValue) override;
	virtual void PostAttributeModify(const FGAEffectContext& InContext, float PreValue, float PostValue) override;
	
	
};
