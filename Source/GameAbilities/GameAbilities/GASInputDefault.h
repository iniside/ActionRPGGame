// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GASInputOverride.h"
#include "GASInputDefault.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASInputDefault : public UGASInputOverride
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeInputPressed(int32 Index) override;
	virtual void NativeInputReleased(int32 Index) override;
};
