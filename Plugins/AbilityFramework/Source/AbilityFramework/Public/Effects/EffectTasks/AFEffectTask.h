// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LatentActions/AFTaskBase.h"
#include "GAEffectExtension.h"
#include "GAGlobalTypes.h"
#include "AFAbilityComponent.h"
#include "AFEffectTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "true") )
class ABILITYFRAMEWORK_API UAFEffectTask : public UAFTaskBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UGAEffectExtension* Effect;
	UPROPERTY()
		class UAFEffectsComponent* EffectsComponent;

public:
	template <class T>
	static T* NewEffectTask(UGAEffectExtension* WorldContextObject, FName InTaskName = FName(), FName InstanceName = FName())
	{
		T* MyObj = nullptr;
		UGAEffectExtension* EffectExtension = WorldContextObject;
		MyObj = NewTask<T, UGAEffectExtension>(WorldContextObject, WorldContextObject, InTaskName);

		MyObj->Effect = EffectExtension;
		MyObj->EffectsComponent = EffectExtension->OwningComponent;

		return MyObj;
	}
};
