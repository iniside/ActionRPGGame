// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "PropertyHandle.h"
/**
 * 
 */
class ABILITYFRAMEWORKEDITOR_API FAFEffectCustomizationCommon
{
public:
	FAFEffectCustomizationCommon();
	~FAFEffectCustomizationCommon();
	static void CreateMagnitudeLayout(IDetailLayoutBuilder& DetailLayout,
		TSharedPtr<IPropertyHandle>& InProperty, FName InCategory);
	static void HideProperty(IDetailLayoutBuilder& DetailLayout, FName InName);
};
