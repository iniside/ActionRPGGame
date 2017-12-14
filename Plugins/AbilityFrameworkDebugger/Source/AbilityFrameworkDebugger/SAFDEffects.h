// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "AFAbilityInterface.h"
/**
 * 
 */
class ABILITYFRAMEWORKDEBUGGER_API SAFDEffects : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAFDEffects)
	{}
	SLATE_END_ARGS()

	IAFAbilityInterface* AFInterface;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
