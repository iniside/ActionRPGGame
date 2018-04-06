// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LatentActions/GALatentFunctionBase.h"
#include "../GAEffectExtension.h"
#include "../../GAGlobalTypes.h"
#include "../../AFAbilityComponent.h"
#include "AFEffectTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "true") )
class ABILITYFRAMEWORK_API UAFEffectTask : public UGALatentFunctionBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UGAEffectExtension* Effect;
	UPROPERTY()
		class UAFEffectsComponent* EffectsComponent;

public:
	template <class T>
	static T* NewEffectTask(UObject* WorldContextObject, FName InTaskName = FName(), FName InstanceName = FName())
	{
		return nullptr;
	}
};
