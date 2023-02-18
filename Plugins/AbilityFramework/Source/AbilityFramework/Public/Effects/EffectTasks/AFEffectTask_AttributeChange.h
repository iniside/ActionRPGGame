// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/EffectTasks/AFEffectTask.h"
#include "AFEffectTask_AttributeChange.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAFTaskAttributeChangedDelegate, FAFAttributeChangedData, Payload);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectTask_AttributeChange : public UAFEffectTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FAFTaskAttributeChangedDelegate OnEvent;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects|Tasks", meta = (HidePin = "OwningExtension", DefaultToSelf = "OwningExtension", BlueprintInternalUseOnly = "TRUE"))
		static UAFEffectTask_AttributeChange* ListenAttributeChanged(UGAEffectExtension* OwningExtension,
			FGAAttribute InAttribute,
			AActor* OptionalExternalTarget = nullptr, 
			bool OnlyTriggerOnce = false);

	UAFEffectTask_AttributeChange(const FObjectInitializer& ObjectInitializer);

	void SetExternalTarget(AActor* Actor);

	class UAFEffectsComponent* GetTargetASC();

	virtual void Activate() override;

	void AttributeChangedCallback(FAFAttributeChangedData Payload);
	
	FGAAttribute Attribute;

	UPROPERTY()
		UAFEffectsComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;

	
	
	
};
