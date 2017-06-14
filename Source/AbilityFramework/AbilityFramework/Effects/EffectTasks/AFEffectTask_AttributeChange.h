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
		static UAFEffectTask_AttributeChange* ListenAttributeChanged(UObject* OwningExtension, 
			FGAAttribute InAttribute,
			AActor* OptionalExternalTarget = nullptr, 
			bool OnlyTriggerOnce = false);

	UAFEffectTask_AttributeChange(const FObjectInitializer& ObjectInitializer);

	void SetExternalTarget(AActor* Actor);

	class UGAAbilitiesComponent* GetTargetASC();

	virtual void Activate() override;

	void AttributeChangedCallback(FAFAttributeChangedData Payload);

	void OnDestroy(bool AbilityEnding) override;

	FGAAttribute Attribute;

	UPROPERTY()
		UGAAbilitiesComponent* OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;

	FDelegateHandle MyHandle;

	
	
	
};
