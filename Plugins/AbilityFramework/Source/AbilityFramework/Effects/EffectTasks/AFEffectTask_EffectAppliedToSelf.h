// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/EffectTasks/AFEffectTask.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "AFEffectTask_EffectAppliedToSelf.generated.h"



/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectTask_EffectAppliedToSelf : public UAFEffectTask
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAFEffectEventDelegate, FAFContextHandle, Context, FAFPropertytHandle, Property, FAFEffectSpecHandle, Spec);
	UPROPERTY(BlueprintAssignable)
		FAFEffectEventDelegate OnEvent;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects|Tasks", meta = (HidePin = "OwningExtension", DefaultToSelf = "OwningExtension", BlueprintInternalUseOnly = "TRUE"))
		static UAFEffectTask_EffectAppliedToSelf* ListenEffectAppliedToSelf(UObject* OwningExtension, FName TaskName, AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false);

	UAFEffectTask_EffectAppliedToSelf(const FObjectInitializer& ObjectInitializer);

	void SetExternalTarget(AActor* Actor);

	class UAFEffectsComponent* GetTargetASC();

	virtual void Activate() override;

	virtual void GameplayEventCallback(FAFContextHandle Context
		, FAFPropertytHandle Property
		, FAFEffectSpecHandle Spec);

	void OnDestroy(bool AbilityEnding) override;
	
	UPROPERTY()
		UAFEffectsComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;
	
};
