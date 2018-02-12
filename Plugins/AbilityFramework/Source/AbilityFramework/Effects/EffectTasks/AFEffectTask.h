// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTask.h"
#include "../GAEffectExtension.h"
#include "../../GAGlobalTypes.h"
#include "../../AFAbilityComponent.h"
#include "AFEffectTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "true") )
class ABILITYFRAMEWORK_API UAFEffectTask : public UGameplayTask
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
		check(WorldContextObject);

		T* MyObj = NewObject<T>(WorldContextObject);
		UGAEffectExtension* ThisAbility = CastChecked<UGAEffectExtension>(WorldContextObject);
	/*	if (UGAAbilityTask* CachedTask = ThisAbility->GetAbilityTask(InTaskName))
		{
			return Cast<T>(CachedTask);
		}*/
		//MyObj->Effect = ThisAbility;
		//MyObj->EffectsComponent = ThisAbility->OwningComponent;
		//MyObj->InitTask(*ThisAbility, ThisAbility->GetGameplayTaskDefaultPriority());
		//MyObj->InstanceName = InstanceName;
		//ThisAbility->AddAbilityTask(InTaskName, MyObj);
		return MyObj;
	}

	template<typename T>
	static bool DelayedFalse()
	{
		return false;
	}

	// this function has been added to make sure AbilityTasks don't use this method
	template <class T>
	FORCEINLINE static T* NewTask(UObject* WorldContextObject, FName InstanceName = FName())
	{
		static_assert(DelayedFalse<T>(), "UAbilityTask::NewTask should never be used. Use NewAbilityTask instead");
	}
};
