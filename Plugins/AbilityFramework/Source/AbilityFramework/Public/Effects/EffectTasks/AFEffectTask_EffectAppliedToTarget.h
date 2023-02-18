// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/EffectTasks/AFEffectTask.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "AFEffectTask_EffectAppliedToTarget.generated.h"



/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectTask_EffectAppliedToTarget : public UAFEffectTask
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAFEffectEventDelegate, FGAEffectContext, Context, FAFPropertytHandle, Property, FAFEffectSpec, Spec);
	UPROPERTY(BlueprintAssignable)
		FAFEffectEventDelegate OnEvent;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects|Tasks", meta = (HidePin = "OwningExtension", DefaultToSelf = "OwningExtension", BlueprintInternalUseOnly = "TRUE"))
		static UAFEffectTask_EffectAppliedToTarget* ListenEffectAppliedToTarget(UGAEffectExtension* OwningExtension, FName TaskName, AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false);

	UAFEffectTask_EffectAppliedToTarget(const FObjectInitializer& ObjectInitializer);

	void SetExternalTarget(AActor* Actor);

	class UAFEffectsComponent* GetTargetASC();

	virtual void Activate() override;

	virtual void GameplayEventCallback(FGAEffectContext Context
		, FAFPropertytHandle Property
		, FAFEffectSpec Spec);

	virtual void OnTaskEnded() override;

	UPROPERTY()
		UAFEffectsComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;
	
};
