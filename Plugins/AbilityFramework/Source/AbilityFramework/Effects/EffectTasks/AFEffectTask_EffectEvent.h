// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/EffectTasks/AFEffectTask.h"
#include "../../GAGlobalTypes.h"
#include "AFEffectTask_EffectEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAFEffectEventDelegate, FAFEventData, Payload);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectTask_EffectEvent : public UAFEffectTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FAFEffectEventDelegate OnEvent;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects|Tasks", meta = (HidePin = "OwningExtension", DefaultToSelf = "OwningExtension", BlueprintInternalUseOnly = "TRUE"))
		static UAFEffectTask_EffectEvent* ListenEffectEvent(UObject* OwningExtension, FGameplayTag EventTag, AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false);

	UAFEffectTask_EffectEvent(const FObjectInitializer& ObjectInitializer);

	void SetExternalTarget(AActor* Actor);

	class UAFAbilityComponent* GetTargetASC();

	virtual void Activate() override;

	virtual void GameplayEventCallback(FAFEventData Payload);

	void OnDestroy(bool AbilityEnding) override;

	FGameplayTag Tag;

	UPROPERTY()
		UAFAbilityComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;
	
};
