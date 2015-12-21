// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GASInputOverride.h"
#include "GASThreeButtonCombo.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASThreeButtonCombo : public UGASInputOverride
{
	GENERATED_BODY()
protected:
	UPROPERTY()
		int32 CurrentAbilityIdx;
public:
	virtual void NativeInputPressed(int32 Index) override;
	virtual void NativeInputReleased(int32 Index) override;
	
};
